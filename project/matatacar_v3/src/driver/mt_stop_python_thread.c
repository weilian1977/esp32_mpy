#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "py/mphal.h"
#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/mphal.h"

#include "mt_log.h"
#include "mt_err.h"
#include "mt_module_config.h"
//#include "system_operation.h"
#include "mp_exec_control.h"
#include "mt_stop_python_thread.h"

#if MODULE_STOP_PYTHON_THREAD_ENABLE
/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define TAG                                  ("stop_python_thread")

#define MP_SOFT_RESTART_WAIT_MAX_TIME        (6000) 

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/  
typedef struct
{
  bool mp_thread_stop_enable;  
  int8_t mp_thread_status[MP_THREAD_MAX];
  uint8_t mp_thread_num;
  /* when this flag is set, means thre are thread shoule be stoped */
  int8_t mp_thread_stop_flag;

  TaskHandle_t mp_thread_handle[MP_THREAD_MAX];
  /* when once operation is completed, this sema will be release */
  SemaphoreHandle_t mp_thread_stop_sema;
  /* indicate the status of thread stoping process */
  int32_t mp_thread_stop_status;
}mp_thread_table_t;

/******************************************************************************
 DECLARE PRIVATE DATAS
 ******************************************************************************/
mp_thread_table_t mp_thread_table = {.mp_thread_stop_enable = false};

#define MODULE_ENABLED_CHECK(ret_val) \
    if((true != mp_thread_table.mp_thread_stop_enable)) \
    { \
      MT_LOGE(TAG, "%s(%d):%s", __FUNCTION__, __LINE__, "stop thread module not init"); \
      return (ret_val); \
    }

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
static stop_thread_err mt_thread_find_id_by_handle_t(TaskHandle_t han, int8_t *thread_id);
static bool mt_thread_once_done_check_t(void);

/******************************************************************************
 DEFINE PUBLIC FUNCTION
 ******************************************************************************/
// int8_t mt_micropython_stop_user_script_t(void)
// {
//   TaskHandle_t mp_task_handle = (TaskHandle_t)mt_system_get_task_handle_t(MICROPYTHON_TASK_ID);
//   if(mp_task_handle == NULL)
//   {
//     MT_LOGE(TAG, "get mp handle error");
//     return -1;
//   }
//   mp_exec_mode_t cur_mode;
//   mp_exec_get_mode_t(&cur_mode);
//   if(cur_mode == MP_EXEC_USER_SCRIPT)
//   {
//     return 0;
//   }
//   mt_thread_stop_thread_by_handle_t(mp_task_handle);

//   mp_exec_set_resatrt_flag_t(SOFT_RESTART_TYPE_NONE);
//   mp_exec_set_mode_t(MP_EXEC_EMPTY_LOOP_SCRIPT);
//   mt_thread_stop_thread_start_schdule_t();
//   return 0;
//  }

// int8_t mt_reboot_micropython_t(int exec_script_id, int restart_mode)
// {
//   TaskHandle_t mp_task_handle = (TaskHandle_t)mt_system_get_task_handle_t(MICROPYTHON_TASK_ID);
//   if(mp_task_handle == NULL)
//   {
//     MT_LOGE(TAG, "get mp handle error");
//     return -1;
//   }

//   mt_thread_stop_thread_by_handle_t(mp_task_handle);

//   mp_exec_set_resatrt_flag_t(restart_mode);
//   mp_exec_set_mode_t(exec_script_id);
//   mt_thread_stop_thread_start_schdule_t();
//   MP_THREAD_GIL_EXIT();
//   if(FUNC_OK == mt_thread_stop_thread_wait_done_t(MP_SOFT_RESTART_WAIT_MAX_TIME))
//   {
//     MT_LOGI(TAG, "soft rester successed");
//     return 0;
//   }
//   else
//   {
//     MT_LOGE(TAG, "soft rester failed");
//     printf(" hardware restart \n");
//     esp_restart();
//     return -1;
//   }
//  }

void mt_thread_exe_hook_t(void)
{
  if(true != mp_thread_table.mp_thread_stop_enable)
  {
    return;
  }
  /* if some thread need to be stopped, this flag will be set true */
  if(mp_thread_table.mp_thread_stop_flag)
  {
    TaskHandle_t current_handle = NULL;
    int8_t id = -1;
    /* get current task handle */
    current_handle = xTaskGetCurrentTaskHandle();
    mt_thread_find_id_by_handle_t(current_handle, &id);
    ESP_LOGI("HOOK", "thread id is %d thread handle is %p", id, current_handle);
    if(id == -1)
    {
      ESP_LOGI("HOOK", " thread is not registered");
      return;
    }
    if(mp_thread_table.mp_thread_status[id] != MP_THREAD_WAIT_TO_RESTART)
    {
      ESP_LOGI("HOOK", " thread do not need to be restart\n\n\n");
      return;
    }
    ESP_LOGI("HOOK", "prerare to restart thread");
    /* we can't execute this line after thread restart, because programming pointer will jump to other place */
    mt_thread_set_status_by_handle_t(current_handle, MP_THREAD_RESTARTED);
    /* check if once operation have finished */
    if(mt_thread_once_done_check_t() == true)
    {
      ESP_LOGI("HOOK", "once stop thread succceed, give the sema");
      mp_thread_table.mp_thread_stop_flag = 0;
      mp_thread_table.mp_thread_stop_status = MP_THREAD_SCHEDULE_RESTARTED;
      xSemaphoreGive(mp_thread_table.mp_thread_stop_sema);
    }
    
    ESP_LOGI("HOOK", "exec mp_handle_pending\n\n\n");

    MP_STATE_THREAD(mp_pending_exception) = MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception));
#if MICROPY_ENABLE_SCHEDULER
    if(MP_STATE_VM(sched_state) == MP_SCHED_IDLE) 
    {
      MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
#endif
  }
}

stop_thread_err mt_thread_table_init_t(void)
{ 
  for(uint8_t i = 0; i < MP_THREAD_MAX; i++)
  {
    mp_thread_table.mp_thread_handle[i] = NULL;
    mp_thread_table.mp_thread_status[i] = MP_THREAD_RESTARTED;
  }
  
  mp_thread_table.mp_thread_num = 0;
  mp_thread_table.mp_thread_stop_flag = 0;
  mp_thread_table.mp_thread_stop_status = MP_THREAD_SCHEDULE_ON_START;
  if(mp_thread_table.mp_thread_stop_sema == NULL)
  {
    mp_thread_table.mp_thread_stop_sema = xSemaphoreCreateBinary();
  }
  if(mp_thread_table.mp_thread_stop_sema == NULL)
  {
    mp_thread_table.mp_thread_stop_enable = false;
    return FUNC_FAIL;
  }
  else
  {
    mp_thread_table.mp_thread_stop_enable = true;
    return FUNC_OK;
  }
}

stop_thread_err mt_thread_table_deinit_t(void)
{ 
  for(uint8_t i = 0; i < MP_THREAD_MAX; i++)
  {
    mp_thread_table.mp_thread_handle[i] = NULL;
    mp_thread_table.mp_thread_status[i] = MP_THREAD_RESTARTED;
  }
  
  mp_thread_table.mp_thread_num = 0;
  mp_thread_table.mp_thread_stop_flag = 0;
  mp_thread_table.mp_thread_stop_status = MP_THREAD_SCHEDULE_ON_START;

  mp_thread_table.mp_thread_stop_enable = false;

  return FUNC_OK;
}

stop_thread_err mt_thread_get_module_status_t(int8_t *status)
{ 
  MODULE_ENABLED_CHECK(FUNC_FAIL);
  (*status) = mp_thread_table.mp_thread_stop_status;
  return FUNC_OK;
}

bool mt_thread_is_enabled_t(void)
{
  return mp_thread_table.mp_thread_stop_enable;
}

uint8_t mt_thread_get_task_number_t(void)
{
  return mp_thread_table.mp_thread_num;
}

stop_thread_err mt_thread_add_thread_to_table_t(TaskHandle_t han, int8_t *thread_id)
{
  MODULE_ENABLED_CHECK(FUNC_FAIL);
  TaskHandle_t current_handle = NULL;
  /* NULL means add current task */
  if(han == NULL)
  {
    current_handle = xTaskGetCurrentTaskHandle();
  }
  else
  {
    current_handle = han;
  }
  
  /* check if the task is existed */
  int8_t id = -1;
  mt_thread_find_id_by_handle_t(current_handle, &id);
  if(id == -1)
  {
    if(mp_thread_table.mp_thread_num >= MP_THREAD_MAX)
    {
      return FUNC_FAIL;
    }
    mp_thread_table.mp_thread_handle[mp_thread_table.mp_thread_num] = current_handle;
    (*thread_id) = mp_thread_table.mp_thread_num;
    //ESP_LOGI(TAG, "thread id is %d, thread handle is %p", mp_thread_table.mp_thread_num, mp_thread_table.mp_thread_handle[mp_thread_table.mp_thread_num]);
    ESP_LOGI(TAG, "thread id is %d, thread handle is %p", mp_thread_table.mp_thread_num, mp_thread_table.mp_thread_handle[mp_thread_table.mp_thread_num]);
    mp_thread_table.mp_thread_num++;
  }
  else
  {
    (*thread_id) = id;
    //ESP_LOGI(TAG, "thread id is %d, thread handle is %p already existed", mp_thread_table.mp_thread_num, mp_thread_table.mp_thread_handle[mp_thread_table.mp_thread_num]);
    ESP_LOGI(TAG, "thread id is %d, thread handle is %p already existed", mp_thread_table.mp_thread_num, mp_thread_table.mp_thread_handle[mp_thread_table.mp_thread_num]);
  }
  return FUNC_OK;
}

stop_thread_err mt_thread_set_status_by_handle_t(TaskHandle_t han, uint8_t sta)
{
  int8_t id = -1;
  mt_thread_find_id_by_handle_t(han, &id);
  if(id != -1)
  { 
    if(mp_thread_table.mp_thread_status[id] == MP_THREAD_FATAL_ERROR)
    {
      return FUNC_FAIL;
    }
    mp_thread_table.mp_thread_status[id] = sta;
    return FUNC_OK;
  }
  else
  {
    return FUNC_FAIL;
  }
}

stop_thread_err mt_thread_set_status_by_id_t(int8_t id, uint8_t sta)
{
  if(id >= 0 && id < mp_thread_table.mp_thread_num)
  {
    /* if fatal error occured in this thread, we can't restart this thread */
    if(mp_thread_table.mp_thread_handle[id] == NULL || mp_thread_table.mp_thread_status[id] == MP_THREAD_FATAL_ERROR)
    {
      return FUNC_FAIL;
    }
    mp_thread_table.mp_thread_status[id] = sta;
    ESP_LOGI(TAG, "mt_thread_set_status_by_id_t thread id is %d, status is %d", id, sta);
    return FUNC_OK;
  }
  else
  {
    return FUNC_FAIL;
  }
}

stop_thread_err mt_thread_get_status_by_id_t(int8_t id, int8_t *status)
{
  if(id >= 0 && id < mp_thread_table.mp_thread_num)
  {
    if(mp_thread_table.mp_thread_handle[id] == NULL || mp_thread_table.mp_thread_status[id] == MP_THREAD_FATAL_ERROR)
    {
      return FUNC_FAIL;
    }
    (*status) = mp_thread_table.mp_thread_status[id];
    return FUNC_OK;
  }
  else
  { 
    return FUNC_FAIL;
  }
}

stop_thread_err mt_thread_get_status_by_handle_t(TaskHandle_t han, int8_t *status)
{
  int8_t id = -1;
  mt_thread_find_id_by_handle_t(han, &id);
  if(id != -1)
  {
    (*status) = mp_thread_table.mp_thread_status[id];
    return FUNC_OK;
  }
  else
  {
    return FUNC_FAIL;
  }
}
stop_thread_err mt_thread_stop_thread_by_handle_t(TaskHandle_t han)
{
  MODULE_ENABLED_CHECK(FUNC_FAIL);
  stop_thread_err res = FUNC_FAIL;
  res = mt_thread_set_status_by_handle_t(han, MP_THREAD_WAIT_TO_RESTART);
  return res;
}

stop_thread_err mt_thread_stop_thread_by_id_t(int8_t id)
{
  MODULE_ENABLED_CHECK(FUNC_FAIL);
  stop_thread_err  res = FUNC_FAIL;
  res = mt_thread_set_status_by_id_t(id, MP_THREAD_WAIT_TO_RESTART);
  return res;

}

stop_thread_err mt_thread_stop_thread_start_schdule_t(void)
{
  MODULE_ENABLED_CHECK(FUNC_FAIL);
  if(mt_thread_once_done_check_t() == true)
  {
    MT_LOGI(TAG, "mt_thread_stop_thread_start_schdule_t, restarr num is 0");
    return FUNC_FAIL;
  }
  else
  {
    mp_thread_table.mp_thread_stop_status = MP_THREAD_SCHEDULE_RESTARTING;
    xSemaphoreTake(mp_thread_table.mp_thread_stop_sema, 0);
    mp_thread_table.mp_thread_stop_flag = 1;
    MT_LOGI(TAG, "mp thread stop thread start schdule");
    return FUNC_OK;
  }
}

stop_thread_err mt_thread_stop_thread_wait_done_t(uint32_t t_ms)
{
  MODULE_ENABLED_CHECK(FUNC_FAIL);
  stop_thread_err res = FUNC_FAIL;
  BaseType_t ret = xSemaphoreTake(mp_thread_table.mp_thread_stop_sema, t_ms / portTICK_PERIOD_MS);

  if(ret == pdTRUE)
  {
    MT_LOGI(TAG, "wait done successed");
    res = FUNC_OK;
  }
  else
  {
    MT_LOGE(TAG, "wait done failed");
    res = FUNC_FAIL;
  }
  
  MT_LOGI(TAG, "once stop thread succceed, give the sema");
  /* wait last thread real restarting done */
  vTaskDelay(50 / portTICK_PERIOD_MS);
  mp_thread_table.mp_thread_stop_flag = 0;
  mp_thread_table.mp_thread_stop_status = MP_THREAD_SCHEDULE_RESTARTED;
  return res;
}

/* there will be a error occure when using the semaphore to make the synchronization */
/* so, use another way to do this in stop scripts function as a temperature solution */
int8_t mt_thread_stop_scripts_wait_done_t(uint32_t t_ms)
{
  int8_t res = 0; 
  uint16_t count = 0;
  while(!mt_thread_once_done_check_t())
  {
    if(count++ > (t_ms / 20))
    {
      res = -1;
      break;
    }
    mp_hal_delay_ms(20);
  }
  /* wait last thread real restarting done */
  mp_hal_delay_ms(50);
  MT_LOGI(TAG, "once stop scripts succceed");
  mp_thread_table.mp_thread_stop_flag = 0;
  mp_thread_table.mp_thread_stop_status = MP_THREAD_SCHEDULE_RESTARTED;
  
  return res;
}

stop_thread_err mt_thread_table_remove_item_by_handle_t(TaskHandle_t han)
{
  int8_t id = -1;
  TaskHandle_t current_handle = NULL;
     
  if(han == NULL)
  {
    current_handle = xTaskGetCurrentTaskHandle();
  }
  else
  {
    current_handle = han;
  }
  
  mt_thread_find_id_by_handle_t(current_handle, &id);
  
  if(id == -1)
  {
    return FUNC_FAIL;
  }
  else
  {
    mp_thread_table.mp_thread_handle[id] = NULL;
    mp_thread_table.mp_thread_status[id] = MP_THREAD_FATAL_ERROR;
    return FUNC_OK;
  }
}

stop_thread_err mt_thread_table_remove_item_by_id_t(int8_t id)
{
  if(id >= 0 && id < mp_thread_table.mp_thread_num)
  {
    mp_thread_table.mp_thread_handle[id] = NULL;
    mp_thread_table.mp_thread_status[id] = MP_THREAD_FATAL_ERROR;
    return FUNC_OK;
  }
  else
  {
    return FUNC_FAIL;
  }
}

/* for stop scripts */
stop_thread_err mt_thread_stop_other_script_t(void)
{
  ESP_LOGI(TAG, "thread id is %d, thread handle is %p already existed", mp_thread_table.mp_thread_num, mp_thread_table.mp_thread_handle[mp_thread_table.mp_thread_num]);
  TaskHandle_t current_handle = NULL;
  current_handle = xTaskGetCurrentTaskHandle();
  /* from 1 not 0 to eliminate main thread  */
  for(uint8_t i = 1; i < mp_thread_table.mp_thread_num; i++)
  {
    ESP_LOGI(TAG, "thread id is %d, thread status is %d", i, mp_thread_table.mp_thread_status[i]);
    if(mp_thread_table.mp_thread_handle[i] == current_handle)
    {
      mt_thread_set_status_by_id_t(i, MP_THREAD_EXECUTING);
    }
    else if(mp_thread_table.mp_thread_handle[i] != NULL)
    {
      ESP_LOGI(TAG, "!= NULL");
      if(mp_thread_table.mp_thread_status[i] != MP_THREAD_RESTARTED
         && mp_thread_table.mp_thread_status[i] != MP_THREAD_FATAL_ERROR)
      {
        ESP_LOGI(TAG, "!= mt_thread_stop_thread_by_id_t");
        mt_thread_stop_thread_by_id_t(i);
      }
    }
  }
  
  mt_thread_stop_thread_start_schdule_t();
  return 0;
}

void mt_thread_stop_self_t(void)
{
  nlr_raise(mp_obj_new_exception(&mp_type_KeyboardInterrupt));
}

void mt_thread_stop_all_script_t(void)
{
  /* this function must be called before stop self */
  mt_thread_stop_other_script_t();
  mt_thread_stop_scripts_wait_done_t(MP_THREAD_STOP_SCRIPT_WAIT_MAX_TIME);
  mt_thread_stop_self_t();
}

void mt_thread_get_current_handle_and_priority_t(void **handle, int *priority)
{
  TaskHandle_t current_handle = NULL;
  current_handle = xTaskGetCurrentTaskHandle();
  (*handle) = current_handle;
  (*priority)  = (int)uxTaskPriorityGet(NULL);

}
/******************************************************************************
 DEFINE PRIVATE FUNCTION
 ******************************************************************************/
static stop_thread_err mt_thread_find_id_by_handle_t(TaskHandle_t han, int8_t *thread_id)
{
  if(han == NULL)
  {
    return FUNC_FAIL;
  }
  for(int8_t i = 0; i < mp_thread_table.mp_thread_num; i++)
  {
    if(han == mp_thread_table.mp_thread_handle[i])
    {
      (*thread_id) = i;
      return FUNC_OK;
    }
  }

  return FUNC_FAIL;
}

static bool mt_thread_once_done_check_t(void)
{
  bool  res = true;
  for(uint8_t i = 0; i < mp_thread_table.mp_thread_num; i++)
  {
    if(mp_thread_table.mp_thread_status[i] == MP_THREAD_WAIT_TO_RESTART)
    {
      res = false;
      break;
    }
  }
  return res;
}

#else /* MODULE_STOP_PYTHON_THREAD_ENABLE */
void mt_thread_exe_hook_t(void)
{

}

#endif /* MODULE_STOP_PYTHON_THREAD_ENABLE */