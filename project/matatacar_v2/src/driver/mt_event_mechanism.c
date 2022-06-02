#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "mt_event_mechanism.h"
#include "mt_module_config.h"
#include "esp_log.h"
#if MODULE_EVENT_ENABLE
/******************************************************************************
 DEFINE MACRO 
 ******************************************************************************/
#define TAG                         ("system_event")

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/  
typedef struct 
{
  mt_module_status_t module_event_status;
  uint16_t eve_trigger_enabled;
  uint16_t mt_eve_cur_num;
  mt_eve_single_t *mt_eve_single[EVE_MAX_NUM];
  bool mt_eve_triggerd[EVENT_TYPE_MAX];
}mt_eve_manager_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static mt_eve_manager_t mt_eve_manager;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
static mt_err_t mt_eve_sync_t(int16_t eve_id);
static bool mt_eve_trigger_check(int16_t eve_id, void *para);

#define EVE_ID_CHECK(a, ret_val) \
    if(a >= mt_eve_manager.mt_eve_cur_num) \
    { \
      MT_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, "eve id invalid"); \
      ESP_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, "eve id invalid"); \
      return (ret_val); \
    }

#define EVE_TYPE_CHECK(a, ret_val) \
    if(a >= EVENT_TYPE_MAX) \
    { \
      MT_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, "eve type invalid"); \
      ESP_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, "eve type invalid"); \
      return (ret_val); \
    }

#define EVE_TRIGGER_TYPE_CHECK(a, ret_val) \
    if(a >= TRIGGER_TYPE_MAX) \
    { \
      MT_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, "eve trigger type invalid"); \
      ESP_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, "eve trigger type invalid"); \
      return (ret_val); \
    }

#define MT_MODULE_EVENT_STATUS_CHECK(a, ret_val) \
    if((a) != mt_eve_manager.module_event_status) \
    { \
      MT_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, "module not init"); \
      ESP_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, "module not init"); \
      return (ret_val); \
    }

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
mt_err_t mt_eve_init_t(void)
{
  memset(&mt_eve_manager, 0, sizeof(mt_eve_manager_t));
  mt_eve_manager.mt_eve_cur_num = 0;
  mt_eve_manager.module_event_status = MT_MODULE_INITIALIZED;
  mt_eve_trigger_disable_t();
  //MT_LOGD(TAG, "event init succeed");

  return MT_OK;
}

mt_err_t mt_eve_deinit_t(void)
{
  for(int16_t i = 0; i < EVE_MAX_NUM; i++)
  {
    if(mt_eve_manager.mt_eve_single[i] != NULL)
    {
      mt_eve_manager.mt_eve_single[i]->eve_status = EVE_STATUS_NOT_REGISTER;
#if FREERTOS_SOPPORT
      if(mt_eve_manager.mt_eve_single[i]->sync_sema)
      {
        vSemaphoreDelete(mt_eve_manager.mt_eve_single[i]->sync_sema);
      }
#endif
      free(mt_eve_manager.mt_eve_single[i]);
      mt_eve_manager.mt_eve_single[i] = NULL; 
    }
  }
  memset(&mt_eve_manager, 0, sizeof(mt_eve_manager_t));
  mt_eve_manager.mt_eve_cur_num = 0;
  mt_eve_manager.module_event_status = MT_MODULE_DEINITIALIZED;
  mt_eve_trigger_disable_t();
  MT_LOGV(TAG, "event deinit succeed");

  return MT_OK;

}

mt_err_t mt_eve_unregister_t(int16_t eve_id)
{
  if(mt_eve_manager.mt_eve_single[eve_id] != NULL)
  {
    MT_LOGI(TAG, "unregister eve id is %d", eve_id);

    mt_eve_manager.mt_eve_single[eve_id]->eve_status = EVE_STATUS_NOT_REGISTER;
#if FREERTOS_SOPPORT
    if(mt_eve_manager.mt_eve_single[eve_id]->sync_sema)
    {
      vSemaphoreDelete(mt_eve_manager.mt_eve_single[eve_id]->sync_sema);
    }
#endif
    free(mt_eve_manager.mt_eve_single[eve_id]);
    mt_eve_manager.mt_eve_single[eve_id] = NULL; 
    mt_eve_manager.mt_eve_cur_num--;
  }

  return MT_OK;
}

mt_err_t mt_eve_register_t(mt_event_type_t eve_type, mt_event_trigger_type_t trigger_type, void *para, event_cb cb, int16_t *id_out)
{
  MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, MT_FAIL);
  EVE_TRIGGER_TYPE_CHECK(trigger_type, MT_ERR_INVALID_ARG);
  EVE_TYPE_CHECK(eve_type, MT_ERR_INVALID_ARG);

  int16_t event_id = -1;
  // find the ungister event firstly
  for(int16_t i = 0; i < EVE_MAX_NUM; i++)
  {
    if(mt_eve_manager.mt_eve_single[i] == NULL)
    {
      event_id = i;
      mt_eve_manager.mt_eve_cur_num++;
      break;
    }
    if(i == EVE_MAX_NUM - 1)
    {
      *id_out = -1;
      return MT_FAIL;
    }
  }
  
  // not found, create a new one
  if(event_id == -1)
  {
    if(mt_eve_manager.mt_eve_cur_num >= EVE_MAX_NUM)
    {
      MT_LOGE(TAG, "event register error");
      (*id_out) = -1;
      return MT_FAIL;
    }

    event_id = mt_eve_manager.mt_eve_cur_num++;
  }

#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
    mt_eve_manager.mt_eve_single[event_id] = (mt_eve_single_t *)heap_caps_malloc(sizeof(mt_eve_single_t), MALLOC_CAP_SPIRAM);
#else
    mt_eve_manager.mt_eve_single[event_id] = (mt_eve_single_t *)malloc(sizeof(mt_eve_single_t));
#endif  

  if(mt_eve_manager.mt_eve_single[event_id] == NULL)
  {
    (*id_out) = -1;
    MT_LOGE(TAG, "eve register failed, alloc memory failed");
    return MT_FAIL;
  }
  mt_eve_manager.mt_eve_single[event_id]->eve_status = EVE_STATUS_REGISTERED;
  mt_eve_manager.mt_eve_single[event_id]->event_type = eve_type;
  mt_eve_manager.mt_eve_single[event_id]->event_id = event_id;
  mt_eve_manager.mt_eve_single[event_id]->event_trigger_type = trigger_type;
  mt_eve_manager.mt_eve_single[event_id]->event_callback = cb;
#if FREERTOS_SOPPORT
  mt_eve_manager.mt_eve_single[event_id]->sync_sema_locked = false;
  mt_eve_manager.mt_eve_single[event_id]->sync_sema = xSemaphoreCreateBinary();
  if(mt_eve_manager.mt_eve_single[event_id]->sync_sema == NULL)
  {
    (*id_out) = -1;
    free(mt_eve_manager.mt_eve_single[event_id]);
    MT_LOGE(TAG, "eve register failed, acreate semaphore fialed");
    return MT_FAIL;
  }
#endif

  if(para != NULL)
  {
    switch(trigger_type)
    {
      case TRIGGER_NEVER:
        /* no parameter */
      break;
      
      case TRIGGER_ALWAYS_WITH_NO_PARAMETER:
        /* no parameter */
      break;

      case TRIGGER_ONCE_BY_VALUE_LARGER:
        /* only support float */
        MT_LOGV(TAG, "register TRIGGER_ONCE_BY_VALUE_LARGER, para: %f", *(float *)para);
        mt_eve_manager.mt_eve_single[event_id]->eve_para.float_val = *(float *)para;
      break;

      case TRIGGER_CONTINUOUS_BY_VALUE_LARGER:
        /* only support float */
        mt_eve_manager.mt_eve_single[event_id]->eve_para.float_val = *(float *)para;
      break;
      
      case TRIGGER_ONCE_BY_VALUE_SMALLER:
        /* only support float */
        mt_eve_manager.mt_eve_single[event_id]->eve_para.float_val = *(float *)para;
      break;

      case TRIGGER_CONTINUOUS_BY_VALUE_SMALLER:
        /* only support float */
        mt_eve_manager.mt_eve_single[event_id]->eve_para.float_val = *(float *)para;
      break;

      case TRIGGER_BY_STRING_MATCHING:
        /* parameter must be a string type */
        strcpy((char *)mt_eve_manager.mt_eve_single[event_id]->eve_para.byte_val, (char *)para);
      break;
      case TRIGGER_ONCE_BY_VALUE_TRUE_WITH_OPTION:
        /* parameter must be a string type */
        memcpy((uint8_t *)mt_eve_manager.mt_eve_single[event_id]->eve_para.byte_val, (uint8_t *)para, 4);
      break;
      default:
        /* no parameter */
      break;
    }
  }

  (*id_out) = event_id;
  
  return MT_OK;
}

mt_err_t mt_eve_redefine_t(int16_t event_id, mt_event_type_t eve_type, mt_event_trigger_type_t trigger_type, void *para, event_cb cb)
{
  MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, MT_FAIL);
  EVE_TRIGGER_TYPE_CHECK(trigger_type, MT_ERR_INVALID_ARG);
  EVE_TYPE_CHECK(eve_type, MT_ERR_INVALID_ARG);

  MT_LOGV(TAG, "modify event definition, eve id is %d, type is %d\n", event_id, eve_type);

  if(mt_eve_manager.mt_eve_single[event_id] == NULL)
  {
    MT_LOGE(TAG, "modify definition error, eve %d is not existed", event_id)
    return MT_FAIL;
  }
  
  mt_eve_manager.mt_eve_single[event_id]->eve_status = EVE_STATUS_REGISTERED;
  mt_eve_manager.mt_eve_single[event_id]->event_type = eve_type;
  mt_eve_manager.mt_eve_single[event_id]->event_id = event_id;
  mt_eve_manager.mt_eve_single[event_id]->event_trigger_type = trigger_type;
  mt_eve_manager.mt_eve_single[event_id]->event_callback = cb;
#if FREERTOS_SOPPORT
  mt_eve_manager.mt_eve_single[event_id]->sync_sema_locked = false;
#endif /* FREERTOS_SOPPORT */
  if(para != NULL)
  {
    switch(trigger_type)
    {
      case TRIGGER_NEVER:
        /* no parameter */
      break;
      
      case TRIGGER_ALWAYS_WITH_NO_PARAMETER:
        /* no parameter */
      break;

      case TRIGGER_ONCE_BY_VALUE_LARGER:
        /* only support float */
        MT_LOGV(TAG, "register TRIGGER_ONCE_BY_VALUE_LARGER, para: %f", *(float *)para);
        mt_eve_manager.mt_eve_single[event_id]->eve_para.float_val = *(float *)para;
      break;

      case TRIGGER_CONTINUOUS_BY_VALUE_LARGER:
        /* only support float */
        mt_eve_manager.mt_eve_single[event_id]->eve_para.float_val = *(float *)para;
      break;
      
      case TRIGGER_ONCE_BY_VALUE_SMALLER:
        /* only support float */
        mt_eve_manager.mt_eve_single[event_id]->eve_para.float_val = *(float *)para;
      break;

      case TRIGGER_CONTINUOUS_BY_VALUE_SMALLER:
        /* only support float */
        mt_eve_manager.mt_eve_single[event_id]->eve_para.float_val = *(float *)para;
      break;

      case TRIGGER_BY_STRING_MATCHING:
        /* parameter must be a string type */
        strcpy((char *)mt_eve_manager.mt_eve_single[event_id]->eve_para.byte_val, (char *)para);
      break;
      case TRIGGER_ONCE_BY_VALUE_TRUE_WITH_OPTION:
        /* parameter must be a string type */
        memcpy((uint8_t *)mt_eve_manager.mt_eve_single[event_id]->eve_para.byte_val, (uint8_t *)para, 4);
      break;
      default:
        /* no parameter */
      break;
    }
  }

  return MT_OK;
}

mt_err_t mt_eve_trigger_by_type_t(mt_event_type_t eve_type, void *para)
{ 
  EVE_TYPE_CHECK(eve_type, MT_ERR_INVALID_ARG);
  //ESP_LOGI(TAG, "mt_eve 0");
  MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, MT_FAIL);
  //ESP_LOGI(TAG, "mt_eve_trigger_by_type_t");
  if(false == mt_eve_manager.eve_trigger_enabled)
  {
    return MT_ERR_INVALID_STATE;
  }
  //ESP_LOGI(TAG, "mt_eve_trigger_by_type_t11");
  mt_err_t ret = MT_FAIL;
  for(int16_t i = 0; i < mt_eve_manager.mt_eve_cur_num; i++)
  {
    if(!mt_eve_manager.mt_eve_single[i])
    {
      continue;
    }
    if(mt_eve_is_sync_locked_t(i))
    {
      continue;
    }
    if(eve_type == mt_eve_manager.mt_eve_single[i]->event_type)
    {
      if(mt_eve_manager.mt_eve_single[i]->eve_status == EVE_STATUS_ENABLE)
      {;
        if(!mt_eve_trigger_check(i, para))
        { 
          continue;
        }
        ret = MT_OK;

        // if being here, means event has occured
        mt_eve_sync_t(i);

        if(mt_eve_manager.mt_eve_single[i]->event_callback)
        {
          mt_eve_manager.mt_eve_single[i]->event_callback();
        }
      }
    }
  }
  return ret;
}

mt_err_t mt_eve_trigger_by_id_t(int16_t eve_id, void *para)
{ 
  MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, MT_FAIL);
  EVE_ID_CHECK(eve_id, MT_ERR_INVALID_ARG);

  if(false == mt_eve_manager.eve_trigger_enabled)
  {
    return MT_ERR_INVALID_STATE;
  }

  if(mt_eve_manager.mt_eve_single[eve_id]->eve_status != EVE_STATUS_ENABLE)
  {
    return MT_ERR_INVALID_STATE;
  }
  
  if(!mt_eve_manager.mt_eve_single[eve_id])
  {
    return MT_FAIL;
  }
  
  if(mt_eve_is_sync_locked_t(eve_id))
  {
    return MT_ERR_INVALID_STATE;
  }
      
  if(mt_eve_trigger_check(eve_id, para))
  {
    // if being here, means event has occured
    mt_eve_sync_t(eve_id);

    if(mt_eve_manager.mt_eve_single[eve_id]->event_callback)
    {
      mt_eve_manager.mt_eve_single[eve_id]->event_callback();
    }
    MT_LOGV(TAG, "eve triggered, id is: %d", eve_id);
  }

  return MT_OK;
}

static mt_err_t mt_eve_sync_t(int16_t eve_id)
{
#if FREERTOS_SOPPORT
  if(mt_eve_manager.mt_eve_single[eve_id]->sync_sema)
  {
    xSemaphoreGive(mt_eve_manager.mt_eve_single[eve_id]->sync_sema);
  }
#endif /* FREERTOS_SOPPORT */
  return MT_OK;
}

/* after callback function executed finished, call this function */
mt_err_t mt_eve_clear_sync_t(int16_t eve_id)
{
#if FREERTOS_SOPPORT
  if(mt_eve_manager.mt_eve_single[eve_id]->sync_sema)
  {
    xSemaphoreTake(mt_eve_manager.mt_eve_single[eve_id]->sync_sema, 0);
  }
  mt_eve_manager.mt_eve_single[eve_id]->sync_sema_locked = false;
#endif /* FREERTOS_SOPPORT */  
  return MT_OK;
}

/* if this function return false, means event callback function is executing */
/* if callback function is executing, we don't need to check the event */
bool mt_eve_is_sync_locked_t(int16_t eve_id)
{
#if FREERTOS_SOPPORT
  return mt_eve_manager.mt_eve_single[eve_id]->sync_sema_locked;
#else
  return false;
#endif
}

mt_err_t mt_eve_wait_triggered_t(int16_t eve_id)
{
  MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, MT_FAIL);
  EVE_ID_CHECK(eve_id, MT_ERR_INVALID_ARG);
#if FREERTOS_SOPPORT  
  if(mt_eve_manager.mt_eve_single[eve_id]->sync_sema == NULL)
  {
    return MT_FAIL;
  }
  
  if(pdTRUE == xSemaphoreTake(mt_eve_manager.mt_eve_single[eve_id]->sync_sema, portMAX_DELAY))
  {
    mt_eve_manager.mt_eve_single[eve_id]->sync_sema_locked = true;
    return MT_OK;
  }
  else
  {
    return MT_FAIL;
  }
#endif /* FREERTOS_SOPPORT */
  return MT_OK;
}

mt_err_t mt_eve_trigger_enable_t(void)
{
  MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, MT_FAIL);
  mt_eve_manager.eve_trigger_enabled = true;
  return MT_OK;
}

mt_err_t mt_eve_trigger_disable_t(void)
{
  //MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, MT_FAIL);
  mt_eve_manager.eve_trigger_enabled = false;
  return MT_OK;
}

bool mt_eve_is_trigger_enabled_t(void)
{  
  MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, false);
  return mt_eve_manager.eve_trigger_enabled;
}

mt_err_t mt_eve_set_status_t(int16_t eve_id, int sta)
{  
  MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, false);
  EVE_ID_CHECK(eve_id, MT_ERR_INVALID_ARG);
  if(mt_eve_manager.mt_eve_single[eve_id])
  {
    mt_eve_manager.mt_eve_single[eve_id]->eve_status = sta;
  }
  return MT_OK;
}

mt_err_t mt_eve_get_status_t(int16_t eve_id, int *sta)
{  
  MT_MODULE_EVENT_STATUS_CHECK(MT_MODULE_INITIALIZED, false);
  EVE_ID_CHECK(eve_id, MT_ERR_INVALID_ARG);
  if(mt_eve_manager.mt_eve_single[eve_id])
  {
    *sta = mt_eve_manager.mt_eve_single[eve_id]->eve_status;
  }
  return MT_OK;
}

int16_t mt_eve_get_event_number_t(void)
{
  return mt_eve_manager.mt_eve_cur_num;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
static void mt_eve_set_triggerd_flag_t(int16_t eve_id, bool sta)
{
  mt_eve_manager.mt_eve_triggerd[eve_id] = sta;
}

static bool mt_eve_get_triggerd_flag_t(int16_t eve_id)
{
  return mt_eve_manager.mt_eve_triggerd[eve_id];
}

static bool mt_eve_trigger_check(int16_t eve_id, void *para)
{
  bool ret = false;

  if(eve_id >= EVE_MAX_NUM)
  {
   return false;
  }
  switch(mt_eve_manager.mt_eve_single[eve_id]->event_trigger_type)
  {
    case TRIGGER_NEVER:
      /* no parameter */
      ret = false;
    break;
    
    case TRIGGER_ALWAYS_WITH_NO_PARAMETER:
      /* no parameter */
      ret = true;
    break;

    case TRIGGER_ONCE_BY_VALUE_TRUE:
      if(*(uint8_t *)para)
      {
        if(false == mt_eve_get_triggerd_flag_t(eve_id))
        {
          mt_eve_set_triggerd_flag_t(eve_id, true);
          ret = true;
        }
        else
        {
          ret = false;
        }
      }
      else
      {
        mt_eve_set_triggerd_flag_t(eve_id, false);
        ret = false;

      }
    break;

    case TRIGGER_CONTINUOUS_BY_VALUE_TRUE:
       if(*(uint8_t *)para)
       {
         ret = true;
       }
       else
       {
         ret = false;
       }
    break;


    case TRIGGER_ONCE_BY_VALUE_LARGER:
      /* only support float */
      MT_LOGV(TAG, "trigger TRIGGER_ONCE_BY_VALUE_LARGER, para: %f, %f", *(float *)para, mt_eve_manager.mt_eve_single[eve_id]->eve_para.float_val);
      if((*(float *)para) > mt_eve_manager.mt_eve_single[eve_id]->eve_para.float_val)
      {
        if(false == mt_eve_get_triggerd_flag_t(eve_id))
        {
          mt_eve_set_triggerd_flag_t(eve_id, true);
          ret = true;
        }
        else
        {
          ret = false;
        }
      }
      else
      {
        mt_eve_set_triggerd_flag_t(eve_id, false);
        ret = false;
      }
    break;

    case TRIGGER_CONTINUOUS_BY_VALUE_LARGER:
      /* only support float */
      if((*(float *)para) > mt_eve_manager.mt_eve_single[eve_id]->eve_para.float_val)
      {
        ret = true;
      }
      else
      {
        ret = false;
      }
    break;
    
    case TRIGGER_ONCE_BY_VALUE_SMALLER:
      /* only support float */
      if((*(float *)para) < mt_eve_manager.mt_eve_single[eve_id]->eve_para.float_val)
      {
        if(false == mt_eve_get_triggerd_flag_t(eve_id))
        {
          mt_eve_set_triggerd_flag_t(eve_id, true);
          ret = true;
        }
        else
        {
          ret = false;
        }
      }
      else
      {
        mt_eve_set_triggerd_flag_t(eve_id, false);
        ret = false;
      }
    break;

    case TRIGGER_CONTINUOUS_BY_VALUE_SMALLER:
      /* only support float */
      if((*(float *)para) < mt_eve_manager.mt_eve_single[eve_id]->eve_para.float_val)
      {
        ret = true;
      }
      else
      {
        ret = false;
      }
    break;

    case TRIGGER_BY_STRING_MATCHING:
      /* parameter must be a string type */
      if(0 == strcmp((char *)(mt_eve_manager.mt_eve_single[eve_id]->eve_para.byte_val), (char *)para))
      {
        ret = true;
      }
      else
      {
        ret = false;
      }
    break;
    
    case TRIGGER_ONCE_BY_VALUE_TRUE_WITH_OPTION:
      {
        int opt_offset = mt_eve_manager.mt_eve_single[eve_id]->eve_para.byte_val[0];
        if(*((uint8_t *)(para) + opt_offset))
        {
          if(false == mt_eve_get_triggerd_flag_t(eve_id))
          {
            mt_eve_set_triggerd_flag_t(eve_id, true);
            ret = true;
          }
          else
          {
            ret = false;
          }
        }
        else
        {
          mt_eve_set_triggerd_flag_t(eve_id, false);
          ret = false;

        }
      }
    break;

    default:
      ret = false;
    break;
  }

  return ret;
}

#endif /* MODULE_EVENT_ENABLE */