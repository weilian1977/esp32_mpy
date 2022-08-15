#ifndef _MB_STOP_PYTHON_THREAD_H_
#define _MB_STOP_PYTHON_THREAD_H_

#define MP_THREAD_STOP_SCRIPT_WAIT_MAX_TIME  (2000) 

#define MP_THREAD_MAX                 (64)
/* for single thread */
#define MP_THREAD_RESTARTED           (0)
#define MP_THREAD_EXECUTING           (1)
#define MP_THREAD_WAIT_TO_RESTART     (2)
#define MP_THREAD_FATAL_ERROR         (3)
/* for the thread stop mechanism */
#define MP_THREAD_SCHEDULE_ON_START           (0)
#define MP_THREAD_SCHEDULE_RESTARTING         (1)
#define MP_THREAD_SCHEDULE_RESTARTED          (2)


#define FUNC_OK 0
#define FUNC_FAIL -1

typedef enum
{ 
  SENSORS_UPDATE_TASK_ID = 0,
  MICROPYTHON_TASK_ID,
  COMMUNICATION_TASK_ID,
  VOICE_TASK_ID,
  COMMUNICATION_SEND_TASK_ID,

  TASK_ID_MAX
}sys_task_id;

typedef int8_t stop_thread_err;

/**
* @brief 
*     - call this function will interrupt script in mp_task,
*     - something should be done in mp_task to make a soft restart
* @param 
*     - exec_script_id
*       the script that will be executed after restart   
*     - restart_mode
*       @reference to mp_soft_restart_type_t   
*
* @return
*     - 0: successed
*     - -1: failed
*/
extern int8_t mt_reboot_micropython_t(int exec_script_id, int restart_mode);

/**
* @brief 
*     - internal function, this function wille be called in micropython VM
*     - #define MICROPY_VM_HOOK_LOOP mt_thread_exe_hook_t();
*     - warning: never call this function in other place except MICROPY_VM_HOOK_LOOP
* @param 
*     - None
*
* @return
*     - None
*/
extern void mt_thread_exe_hook_t(void);

/**
* @brief 
*     - there is a structure stores mp thread handle and executing status, and so on
*     - call this function to initialize the whole structure;
*     - warning: everytime restart micropython this function should be called once
* @param 
*     - None
*
* @return
*     - FUNC_OK: init success
*     - FUNC_FAIL: init failed, semaphore can't be cerated
*/
extern stop_thread_err mt_thread_table_init_t(void);

/**
* @brief 
*     - call this function to deinit the whole structure;
* @param 
*     - None
*
* @return
*     - FUNC_OK: deinit success
*/
extern stop_thread_err mt_thread_table_deinit_t(void);

/**
* @brief 
*     - call this function to get status of once thread control
* @param 
*     - status: status out
* @return
*     - FUNC_OK: 
*     - FUNC_FAIL: module not enable
*/
extern stop_thread_err mt_thread_get_module_status_t(int8_t *status);

/**
* @brief 
*     - call this function to judge if this module is enabled 
* @param 
*     - None
* @return
*     - true: enabled
*     - false: not enabled, related function will not 
*/
extern bool mt_thread_is_enabled_t(void);

/**
* @brief 
*     - call this function to add python task to table
*     - warning: never add non python thread handle
* @param 
*     - han:
*       -- task handle, if han is NULL, means add current task
*     - thread_id:
*       -- thread_id out, -1 means add failed
* @return
*     - FUNC_OK: add successed
*     - FUNC_FAIL: add failed, table is full will get this ret
*/
extern stop_thread_err mt_thread_add_thread_to_table_t(TaskHandle_t han, int8_t *thread_id);

/**
* @brief 
*     - call this function to set the status of mp thread to CODEY_THREAD_WAIT_TO_RESTART
*     - use id that binded to handle
* @param 
*     - id: id that binded to handle
* @return
*     -- FUNC_FAIL: failed
*     -- FUNC_OK:  succeed
*/
extern stop_thread_err mt_thread_stop_thread_by_id_t(int8_t id);

/**
* @brief 
*     - call this function to set the status of mp thread to CODEY_THREAD_WAIT_TO_RESTART
*     - use ireal handle
* @param 
*     - han: mp thread handle
* @return
*     -- FUNC_FAIL: failed
*     -- FUNC_OK:  succeed
*/
extern stop_thread_err mt_thread_stop_thread_by_handle_t(TaskHandle_t han);

/**
* @brief 
*     - call this function to start once operation
* @param 
*     - None
* @return
*     -- FUNC_FAIL: no thread need to be stoped
*     -- FUNC_OK:  succeed
*/
extern stop_thread_err mt_thread_stop_thread_start_schdule_t(void);

/**
* @brief 
*     - only call this function after mt_mpy_thread_stop_thread_start_schdule_t
*     - once mp thread restart operation completed, the synchronization semaphore will be released
*     - this function will suspend the task until once operation completed or the time exceed
* @param 
*     - t_ms: max wait time, unit is ms
* @return
*     -- FUNC_FAIL: failed, wait max time, and semaphore not released
*     -- FUNC_OK:  succeed
*/
extern stop_thread_err mt_thread_stop_thread_wait_done_t(uint32_t t_ms);



/* there will be a error occure when using the semaphore to make the synchronization */
/* so, use another way to do this in stop scripts function as a temperature solution */
extern int8_t mt_thread_stop_scripts_wait_done_t(uint32_t t_ms);
extern stop_thread_err mt_thread_table_remove_item_by_handle_t(TaskHandle_t han);
extern stop_thread_err mt_thread_table_remove_item_by_id_t(int8_t id);

/* for stop scripts */
extern stop_thread_err mt_thread_stop_other_script_t(void);
extern void mt_thread_stop_self_t(void);
extern void mt_thread_stop_all_script_t(void);

stop_thread_err mt_thread_set_status_by_handle_t(TaskHandle_t han, uint8_t sta);
stop_thread_err mt_thread_set_status_by_id_t(int8_t id, uint8_t sta);
stop_thread_err mt_thread_get_status_by_id_t(int8_t id, int8_t *status);
stop_thread_err mt_thread_get_status_by_handle_t(TaskHandle_t han, int8_t *status);

void mt_thread_get_current_handle_and_priority_t(void **handle, int *priority);

#endif /* _MB_ESP32_STOP_PYTHON_THREAD_H_ */