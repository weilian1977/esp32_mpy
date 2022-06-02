#ifndef _MT_EVENT_H_
#define _MT_EVENT_H_

#define FREERTOS_SOPPORT   1
#define MT_PLATFORM_SUPPORT 1

#if FREERTOS_SOPPORT
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "freertos/semphr.h"
  #include "freertos/queue.h"
#endif /* FREERTOS_SOPPORT */

  #include "mt_err.h"
  #include "mt_log.h"

#define EVE_MAX_NUM                 (64)
#define EVE_PARAMETER_BYTE_MAX_SIZE (64)

typedef void (*event_cb)(void);

/* trigger type, different type means 
 * different way to check the parameter
 */
typedef enum
{
  TRIGGER_NEVER = 0,                  // the event of this type will be never trriggered
  TRIGGER_ALWAYS_WITH_NO_PARAMETER,   // the event of this type will be always trriggered
  TRIGGER_ONCE_BY_VALUE_TRUE,         // trigger once when the condition comes true from false
  TRIGGER_CONTINUOUS_BY_VALUE_TRUE,   // trigger multiple times when the condition being true
  TRIGGER_ONCE_BY_VALUE_LARGER,       // trigger once when the threshold is larger than the parameter in
  TRIGGER_CONTINUOUS_BY_VALUE_LARGER, // trigger multiple times when the threshold is larger than the parameter in
  TRIGGER_ONCE_BY_VALUE_SMALLER,      // trigger once when the threshold is smaller than the parameter in
  TRIGGER_CONTINUOUS_BY_VALUE_SMALLER,// trigger multiple times when the threshold is smaller than the parameter in
  TRIGGER_BY_STRING_MATCHING,         // trigger once when the target string is matched with the parameter in
  TRIGGER_ONCE_BY_VALUE_TRUE_WITH_OPTION,  // a special type, we do not use it now
  TRIGGER_TYPE_MAX
}mt_event_trigger_type_t;

/* event type 
 * you can define your event type here, but number 1 to 10 is reserved; 
 * one type mens one condition, and should bind to one trigger type @reference:mt_event_trigger_type_t
 */
typedef enum
{
  EVE_SYSTEM_LAUNCH = 1,
  EVE_TIME_OVER = 2,
  EVE_MESSAGE = 3,
  EVE_CLOUD_MESSAGE = 4,
  EVE_MESH_MESSAGE = 5,
  EVE_UPLOAD_MODE_MESSAGE = 6,
  EVE_SYSTEM_RUN_APP = 7,
  /* 1 - 10 is reserved, please add event type larger then 10 */
  /* ex: EVE_BUTTON_PRESS = 11 */
  EVENT_BUTTON = 11,
  EVENT_TOUCHPAD_0,
  EVENT_TOUCHPAD_1,
  EVENT_TOUCHPAD_2,
  EVENT_TOUCHPAD_3,

  EVENT_BUTTON_A_PRESSED,
  EVENT_BUTTON_A_RELEASED,
  EVENT_BUTTON_B_PRESSED,
  EVENT_BUTTON_B_RELEASED,
  EVENT_BUTTON_PLAY_PRESSED,
  EVENT_BUTTON_PLAY_RELEASED,

  EVENT_LIGHT_LEFT_LESS,
  EVENT_LIGHT_LEFT_MORE,
  EVENT_LIGHT_RIGHT_LESS,
  EVENT_LIGHT_RIGHT_MORE,

  EVENT_SHAKED,
  EVENT_TILT_LEFT,
  EVENT_TILT_RIGHT,
  EVENT_TILT_FORWARD,
  EVENT_TILT_BACKWARD,
  EVENT_ROTATE_CLOCKWISE,
  EVENT_ROTATE_ANTICLOCKWISE,
  EVENT_FREE_FALL,

  EVENT_MICROPHONE,

  EVENT_TYPE_MAX
}mt_event_type_t;

/* the status of a event
 * you can dynamically change the event status 
 */
typedef enum
{
  EVE_STATUS_NOT_REGISTER = 0,
  EVE_STATUS_REGISTERED,
  EVE_STATUS_ENABLE,
  EVE_STATUS_DISABLE,
}mt_single_eve_status;

/* the structure of a event, it contains all the information */
typedef struct
{
  int16_t event_id;                              // every event should be binded to a event_id
  volatile int      eve_status;                  // @reference:mt_single_eve_status
  mt_event_type_t    event_type;                 // @reference:mt_event_type_t
  mt_event_trigger_type_t event_trigger_type;    // @reference:mt_event_trigger_type_t 
#if FREERTOS_SOPPORT
  SemaphoreHandle_t  sync_sema;                  // this semaphone inform someone that the event has triggered 
  int32_t            sync_sema_locked;           // get this flag to check if the callback is executing, if so, just return
#endif /* #if FREERTOS_SOPPORT */
  event_cb event_callback;                       // when the event is triggered, it comes to this function
  union eve_para_t                               // the target parameter(trigger condition) of this event, parameter can corretly parsed with a specific trigger type
  {
    uint8_t byte_val[EVE_PARAMETER_BYTE_MAX_SIZE];
    float   float_val; 
    int     int_val; 
  }eve_para;
}mt_eve_single_t;


/**
* @brief 
*     - this function must be called before you use the event module,
*     - it will create the necessary resource, and initialize the module 
* @param 
*     - None
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_init_t(void);
/**
* @brief 
*     - remove this module, and free the resource
* @param 
*     - None
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_deinit_t(void);
/**
* @brief 
*     - enable event trigger
* @note:
*     - when mt_eve_init_t or mt_eve_deinit_t is called, event triggering is disabled,
*       so, you need call this function to start the event system
* @param 
*     - 
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_trigger_enable_t(void);
/**
* @brief 
*     - enable event triggering
* @param 
*     - None
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_trigger_disable_t(void);
/**
* @brief 
*     - check if triggering is enabled
* @param 
*     - None
* @return
*     - true: is enable
*     - false: is disable
*/
extern bool mt_eve_is_trigger_enabled_t(void);
/**
* @brief 
*     - register a event, eve_type, trigger_type are necessary, some event type without para, eg: EVE_SYSTEM_LAUNCH,
*       cb is an option, in usual, just make it a NULL value. 
* @param 
*     - eve_type
*      - @reference:mt_event_type_t
*     - mt_event_trigger_type_t
*      - @reference:mt_event_trigger_type_t
*     - para
*      - the target condition to trigger this event, it will be parsed with the specific trigger type.
*     - cb
*      - an option, when the event is truely triggered, this function will be called 
*     - id_out
*      - if a event register successfully, a invalid id(not -1) will be return 
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_register_t(mt_event_type_t eve_type, mt_event_trigger_type_t mt_event_trigger_type_t, void *para, event_cb cb, int16_t *id_out);

/**
* @brief 
*     - unregister an event
* @param 
*     - eve_id
*      - the index that register function returned
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_unregister_t(int16_t eve_id);
/**
* @brief 
*     - you can redefine the event with this function, it's a better way to restart the event system.
* @param 
*     - event_id
*      - the event id you need to redefine it 
*     - eve_type
*      - @reference:mt_event_type_t
*     - mt_event_trigger_type_t
*      - @reference:mt_event_trigger_type_t
*     - para
*      - the target condition to trigger this event, it will be parsed with the specific trigger type.
*     - cb
*      - an option, when the event is truely triggered, this function will be called 
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_redefine_t(int16_t event_id, mt_event_type_t eve_type, mt_event_trigger_type_t trigger_type, void *para, event_cb cb);
/**
* @brief 
*     - you can call this function anywhere, but you can only trigger one event
* @param 
*     - eve_id
*      - the event id you need to trigger it 
*     - para
*      - the parameters that will be compared with the target condition that defines in mt_eve_register_t
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_trigger_by_id_t(int16_t eve_id, void *para);
/**
* @brief 
*     - you can call this function anywhere, it will trigger the events which type is eve_type
* @param 
*     - eve_type
*      - @reference:mt_event_type_t
*     - para
*      - the parameters that will be compared with the target condition that defines in mt_eve_register_t
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_trigger_by_type_t(mt_event_type_t eve_type, void *para);

/**
* @brief 
*     - set the status of a specific event 
* @param 
*     - eve_id
*      - event id
*     - sta
*      - @reference:mt_single_eve_status
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_set_status_t(int16_t eve_id, int sta);
/**
* @brief 
*     - get the status of a specific event 
* @param 
*     - eve_id
*      - event id
*     - sta
*      - @reference:mt_single_eve_status, status output
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_get_status_t(int16_t eve_id, int *sta);

/**
* @brief 
*     - after callback function executed finished, call this function
* @param 
*     - eve_id
*      - event id
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_clear_sync_t(int16_t eve_id);
/**
* @brief 
*     - if this function return false, means event callback function is executing
*       if callback function is executing, we don't need to check the event
* @param 
*     - eve_id
*      - event id
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern bool mt_eve_is_sync_locked_t(int16_t eve_id);
/**
* @brief 
*     - wait the event triggered, used a semaphone here
* @param 
*     - eve_id
*      - event id
* @return
*     - MT_OK: successed
*     - MT_FAIL: failed
*/
extern mt_err_t mt_eve_wait_triggered_t(int16_t eve_id);
/**
* @brief 
*     - get the current event number, some invalid event will not be counted
* @param 
*     - None
* @return
*     - event number
*/
extern int16_t mt_eve_get_event_number_t(void);

#endif /* _MT_EVENT_H_ */


