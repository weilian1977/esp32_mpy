#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/nlr.h"
#include "py/obj.h"

#include "mt_err.h"
#include "mt_log.h"
#include "mt_module_config.h"
#include "mt_event_mechanism.h"
#include "mt_mpy_event.h"

#include "esp_log.h"

#if MODULE_EVENT_ENABLE
/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define TAG                           ("mpy_event")

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/  
typedef struct
{
  mp_obj_base_t base;
}mt_mpy_event_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATAS
 ******************************************************************************/
static mt_mpy_event_obj_t mt_mpy_event_obj = {.base = {&mt_mpy_event_type}};

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS 
 ******************************************************************************/


/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/


/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/

/* binding to micropython */
static mp_obj_t mt_mpy_event_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args)
{
  // parse args
  mp_map_t kw_args;
  mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
  
  // setup the object
  mt_mpy_event_obj_t *self = &mt_mpy_event_obj;
  self->base.type = &mt_mpy_event_type;
  
  return self;
}

static mp_obj_t mt_mpy_event_init(mp_obj_t self_in)
{
  mt_eve_init_t();
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_event_init_obj, mt_mpy_event_init);

static mp_obj_t mt_mpy_event_deinit(mp_obj_t self_in)
{
  mt_eve_deinit_t();
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_event_deinit_obj, mt_mpy_event_deinit);

static mp_obj_t mt_mpy_event_trigger_enable(mp_obj_t self_in)
{
  mt_eve_trigger_enable_t();
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_event_trigger_enable_obj, mt_mpy_event_trigger_enable);

static mp_obj_t mt_mpy_event_trigger_disable(mp_obj_t self_in)
{
  mt_eve_trigger_disable_t();
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_event_trigger_disable_obj, mt_mpy_event_trigger_disable);

static mp_obj_t mt_mpy_event_set_status(mp_obj_t self_in, mp_obj_t arg1, mp_obj_t arg2)
{
  int16_t eve_id = mp_obj_get_int(arg1);
  int sta = mp_obj_get_int(arg2);
  mt_eve_set_status_t(eve_id, sta);
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_3(mt_mpy_event_set_status_obj, mt_mpy_event_set_status);

static mp_obj_t mt_mpy_event_get_status(mp_obj_t self_in, mp_obj_t arg1)
{
  int16_t eve_id = mp_obj_get_int(arg1);
  int sta = 0;
  mt_eve_get_status_t(eve_id, &sta);

  return mp_obj_new_int(sta);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_event_get_status_obj, mt_mpy_event_get_status);

static mp_obj_t mt_mpy_event_test(mp_obj_t self_in)
{
  ESP_LOGI(TAG, "mt_mpy_event_test");
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_event_test_obj, mt_mpy_event_test);

static mp_obj_t mt_mpy_event_register(size_t n_args, const mp_obj_t *args)
{
  ESP_LOGI(TAG, "mt_mpy_event_register");
  int32_t eve_type = mp_obj_get_int(args[1]);
  int32_t trigger_type = mp_obj_get_int(args[2]);
  int16_t register_id = -1;
  uint8_t para_data[EVE_PARAMETER_BYTE_MAX_SIZE];

  if(mp_obj_is_float(args[3]))
  {
    float temp = mp_obj_get_float(args[3]);
    memcpy(para_data, &temp, 4);
  }
  else if(MP_OBJ_IS_INT(args[3]))
  {
    float temp = (float)(mp_obj_get_int(args[3]));
    memcpy(para_data, &temp, 4);
  }
  else if(MP_OBJ_IS_STR(args[3]))
  {        
    size_t len;
    const char *str = mp_obj_str_get_data(args[3], &len);
    if(len > EVE_PARAMETER_BYTE_MAX_SIZE)
    {
      return mp_obj_new_int(-1);
    }
    else
    {
      strcpy((char *)para_data, str);
    }
    
  }
  else
  {
    /* no parameter */
  }
  
  if(n_args >= 5)
  {
    /* callback */
  }

  mt_eve_register_t(eve_type, trigger_type, para_data, NULL, &register_id);
  ESP_LOGI(TAG, "event register id is: %d", register_id);
  ESP_LOGI(TAG, "eve_type is: %d", eve_type);
  ESP_LOGI(TAG, "trigger_type is: %d", trigger_type);
  
  return mp_obj_new_int(register_id);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mt_mpy_event_register_obj, 4, 6, mt_mpy_event_register);

static mp_obj_t mt_mpy_event_unregister(mp_obj_t self_in, mp_obj_t arg1)
{
  int32_t eve_id = mp_obj_get_int(arg1);

  mt_eve_unregister_t(eve_id);
  MT_LOGD(TAG, "event register id is: %d", eve_id);
  ESP_LOGI(TAG, "event unregister id is: %d !!!", eve_id);
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_event_unregister_obj, mt_mpy_event_unregister);


static mp_obj_t mt_mpy_event_redefine(size_t n_args, const mp_obj_t *args)
{
  int16_t  eve_id =  mp_obj_get_int(args[1]);
  int32_t eve_type = mp_obj_get_int(args[2]);
  int32_t trigger_type = mp_obj_get_int(args[3]);

  uint8_t para_data[EVE_PARAMETER_BYTE_MAX_SIZE];

  if(mp_obj_is_float(args[4]))
  {
    float temp = mp_obj_get_float(args[4]);
    memcpy(para_data, &temp, 4);
  }
  else if(MP_OBJ_IS_INT(args[4]))
  {
    float temp = (float)(mp_obj_get_int(args[4]));
    memcpy(para_data, &temp, 4);
  }
  else if(MP_OBJ_IS_STR(args[4]))
  {        
    size_t len;
    const char *str = mp_obj_str_get_data(args[4], &len);
    if(len > EVE_PARAMETER_BYTE_MAX_SIZE)
    {
      return mp_obj_new_int(-1);
    }
    else
    {
      strcpy((char *)para_data, str);
    }
    
  }
  else
  {
    /* no parameter */
  }
  
  if(n_args >= 5)
  {
    /* callback */
  }

  mt_eve_redefine_t(eve_id, eve_type, trigger_type, para_data, NULL);
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mt_mpy_event_redefine_obj, 4, 6, mt_mpy_event_redefine);

static mp_obj_t mt_mpy_event_trigger_by_id(mp_obj_t self_in, mp_obj_t arg1, mp_obj_t arg2)
{
  int16_t eve_id = mp_obj_get_int(arg1);
  uint8_t para_data[EVE_PARAMETER_BYTE_MAX_SIZE];

  if(mp_obj_is_float(arg2))
  {
    float temp = mp_obj_get_float(arg2);
    memcpy(para_data, &temp, 4);
  }
  else if(mp_obj_is_integer(arg2))
  {
    float temp = (float)mp_obj_get_int(arg2);
    memcpy(para_data, &temp, 4);
  }
  else if(MP_OBJ_IS_STR(arg2))
  {        
    size_t len;
    const char *str = mp_obj_str_get_data(arg2, &len);
    if(len > EVE_PARAMETER_BYTE_MAX_SIZE)
    {
      return mp_const_false;
    }
    else
    {
      strcpy((char *)para_data, str);
    }
    
  }
  else
  {
    return mp_const_false;
  }

  mt_eve_trigger_by_id_t(eve_id, para_data);
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_3(mt_mpy_event_trigger_by_id_obj, mt_mpy_event_trigger_by_id);

static mp_obj_t mt_mpy_event_trigger_by_type(mp_obj_t self_in, mp_obj_t arg1, mp_obj_t arg2)
{
  int eve_type = mp_obj_get_int(arg1);
  uint8_t para_data[EVE_PARAMETER_BYTE_MAX_SIZE];

  if(mp_obj_is_float(arg2))
  {
    float temp = mp_obj_get_float(arg2);
    memcpy(para_data, &temp, 4);
  }
  else if(mp_obj_is_integer(arg2))
  {
    float temp = (float)mp_obj_get_int(arg2);
    memcpy(para_data, &temp, 4);
  }
  else if(MP_OBJ_IS_STR(arg2))
  {        
    size_t len;
    const char *str = mp_obj_str_get_data(arg2, &len);
    if(len > EVE_PARAMETER_BYTE_MAX_SIZE)
    {
      return mp_const_false;
    }
    else
    {
      strcpy((char *)para_data, str);
    }
    
  }
  else
  {
    ;
  }
  MT_LOGD(TAG, "trigger tpye is:%d", eve_type);
  mt_eve_trigger_by_type_t(eve_type, para_data);
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_3(mt_mpy_event_trigger_by_type_obj, mt_mpy_event_trigger_by_type);

static mp_obj_t mt_mpy_event_clear_sync(mp_obj_t self_in, mp_obj_t arg1)
{
  int16_t eve_id = mp_obj_get_int(arg1);
  mt_eve_clear_sync_t(eve_id);
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_event_clear_sync_obj, mt_mpy_event_clear_sync);

static mp_obj_t mt_mpy_event_wait_trigger(mp_obj_t self_in, mp_obj_t arg1)
{
  int16_t eve_id = mp_obj_get_int(arg1);
  MP_THREAD_GIL_EXIT();
  if(MT_OK == mt_eve_wait_triggered_t(eve_id))
  {
    MP_THREAD_GIL_ENTER();
    return mp_const_true;
  }
  else
  {
    MP_THREAD_GIL_ENTER();
    return mp_const_false;
  }

}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_event_wait_trigger_obj, mt_mpy_event_wait_trigger);

static void mt_mpy_event_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{

}

static mp_obj_t mt_mpy_event_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args)
{
  mp_arg_check_num(n_args, n_kw, 0, 0, false);
  
  return mp_const_none;
}

static const mp_map_elem_t mt_mpy_event_locals_dict_table[] =
{
  { MP_OBJ_NEW_QSTR(MP_QSTR_test),                   (mp_obj_t)&mt_mpy_event_test_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_init),                   (mp_obj_t)&mt_mpy_event_init_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),                 (mp_obj_t)&mt_mpy_event_deinit_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_event_register),         (mp_obj_t)&mt_mpy_event_register_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_event_unregister),       (mp_obj_t)&mt_mpy_event_unregister_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_event_redefine),         (mp_obj_t)&mt_mpy_event_redefine_obj },  
  { MP_OBJ_NEW_QSTR(MP_QSTR_trigger_by_id),          (mp_obj_t)&mt_mpy_event_trigger_by_id_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_trigger_by_type),        (mp_obj_t)&mt_mpy_event_trigger_by_type_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_wait_trigger),           (mp_obj_t)&mt_mpy_event_wait_trigger_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_trigger_enable),         (mp_obj_t)&mt_mpy_event_trigger_enable_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_trigger_disable),        (mp_obj_t)&mt_mpy_event_trigger_disable_obj },  
  { MP_OBJ_NEW_QSTR(MP_QSTR_set_status),             (mp_obj_t)&mt_mpy_event_set_status_obj }, 
  { MP_OBJ_NEW_QSTR(MP_QSTR_get_status),             (mp_obj_t)&mt_mpy_event_get_status_obj },   
  { MP_OBJ_NEW_QSTR(MP_QSTR_clear_sync),             (mp_obj_t)&mt_mpy_event_clear_sync_obj },  
  
  /* const variables */
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_NEVER),                          MP_OBJ_NEW_SMALL_INT(TRIGGER_NEVER) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_ALWAYS_WITH_NO_PARAMETER),       MP_OBJ_NEW_SMALL_INT(TRIGGER_ALWAYS_WITH_NO_PARAMETER) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_ONCE_BY_VALUE_TRUE),             MP_OBJ_NEW_SMALL_INT(TRIGGER_ONCE_BY_VALUE_TRUE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_CONTINUOUS_BY_VALUE_TRUE),       MP_OBJ_NEW_SMALL_INT(TRIGGER_CONTINUOUS_BY_VALUE_TRUE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_ONCE_BY_VALUE_LARGER),           MP_OBJ_NEW_SMALL_INT(TRIGGER_ONCE_BY_VALUE_LARGER) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_CONTINUOUS_BY_VALUE_LARGER),     MP_OBJ_NEW_SMALL_INT(TRIGGER_CONTINUOUS_BY_VALUE_LARGER) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_ONCE_BY_VALUE_SMALLER),          MP_OBJ_NEW_SMALL_INT(TRIGGER_ONCE_BY_VALUE_SMALLER) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_CONTINUOUS_BY_VALUE_SMALLER),    MP_OBJ_NEW_SMALL_INT(TRIGGER_CONTINUOUS_BY_VALUE_SMALLER) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_BY_STRING_MATCHING),             MP_OBJ_NEW_SMALL_INT(TRIGGER_BY_STRING_MATCHING) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_ONCE_BY_VALUE_TRUE_WITH_OPTION), MP_OBJ_NEW_SMALL_INT(TRIGGER_ONCE_BY_VALUE_TRUE_WITH_OPTION) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TRIGGER_TYPE_MAX),                       MP_OBJ_NEW_SMALL_INT(TRIGGER_TYPE_MAX) },

  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_SYSTEM_LAUNCH),                      MP_OBJ_NEW_SMALL_INT(EVE_SYSTEM_LAUNCH) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_TIME_OVER),                          MP_OBJ_NEW_SMALL_INT(EVE_TIME_OVER) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_MESSAGE),                            MP_OBJ_NEW_SMALL_INT(EVE_MESSAGE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_CLOUD_MESSAGE),                      MP_OBJ_NEW_SMALL_INT(EVE_CLOUD_MESSAGE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_MESH_MESSAGE),                       MP_OBJ_NEW_SMALL_INT(EVE_MESH_MESSAGE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_UPLOAD_MODE_MESSAGE),                MP_OBJ_NEW_SMALL_INT(EVE_UPLOAD_MODE_MESSAGE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_SYSTEM_RUN_APP),                     MP_OBJ_NEW_SMALL_INT(EVE_SYSTEM_RUN_APP) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_BUTTON),                           MP_OBJ_NEW_SMALL_INT(EVENT_BUTTON) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_BUTTON_A_PRESSED),                 MP_OBJ_NEW_SMALL_INT(EVENT_BUTTON_A_PRESSED) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_BUTTON_A_RELEASED),                MP_OBJ_NEW_SMALL_INT(EVENT_BUTTON_A_RELEASED) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_BUTTON_B_PRESSED),                 MP_OBJ_NEW_SMALL_INT(EVENT_BUTTON_B_PRESSED) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_BUTTON_B_RELEASED),                MP_OBJ_NEW_SMALL_INT(EVENT_BUTTON_B_RELEASED) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_BUTTON_PLAY_PRESSED),              MP_OBJ_NEW_SMALL_INT(EVENT_BUTTON_PLAY_PRESSED) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_BUTTON_PLAY_RELEASED),             MP_OBJ_NEW_SMALL_INT(EVENT_BUTTON_PLAY_RELEASED) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_LIGHT_LEFT_LESS),                  MP_OBJ_NEW_SMALL_INT(EVENT_LIGHT_LEFT_LESS) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_LIGHT_LEFT_MORE),                  MP_OBJ_NEW_SMALL_INT(EVENT_LIGHT_LEFT_MORE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_LIGHT_RIGHT_LESS),                 MP_OBJ_NEW_SMALL_INT(EVENT_LIGHT_RIGHT_LESS) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_LIGHT_RIGHT_MORE),                 MP_OBJ_NEW_SMALL_INT(EVENT_LIGHT_RIGHT_MORE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_TOUCHPAD_0),                       MP_OBJ_NEW_SMALL_INT(EVENT_TOUCHPAD_0) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_TOUCHPAD_1),                       MP_OBJ_NEW_SMALL_INT(EVENT_TOUCHPAD_1) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_TOUCHPAD_2),                       MP_OBJ_NEW_SMALL_INT(EVENT_TOUCHPAD_2) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_TOUCHPAD_3),                       MP_OBJ_NEW_SMALL_INT(EVENT_TOUCHPAD_3) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_SHAKED),                           MP_OBJ_NEW_SMALL_INT(EVENT_SHAKED) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_TILT_LEFT),                        MP_OBJ_NEW_SMALL_INT(EVENT_TILT_LEFT) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_TILT_RIGHT),                       MP_OBJ_NEW_SMALL_INT(EVENT_TILT_RIGHT) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_TILT_FORWARD),                     MP_OBJ_NEW_SMALL_INT(EVENT_TILT_FORWARD) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_TILT_BACKWARD),                    MP_OBJ_NEW_SMALL_INT(EVENT_TILT_BACKWARD) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_ROTATE_CLOCKWISE),                 MP_OBJ_NEW_SMALL_INT(EVENT_ROTATE_CLOCKWISE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_ROTATE_ANTICLOCKWISE),             MP_OBJ_NEW_SMALL_INT(EVENT_ROTATE_ANTICLOCKWISE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_FREE_FALL),                        MP_OBJ_NEW_SMALL_INT(EVENT_FREE_FALL) },

  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_MICROPHONE),                       MP_OBJ_NEW_SMALL_INT(EVENT_MICROPHONE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_TYPE_MAX),                         MP_OBJ_NEW_SMALL_INT(EVENT_TYPE_MAX) },

  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_MAX_NUM),                            MP_OBJ_NEW_SMALL_INT(EVE_MAX_NUM) },
  
  // event status
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_STATUS_NOT_REGISTER),                MP_OBJ_NEW_SMALL_INT(EVE_STATUS_NOT_REGISTER) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_STATUS_REGISTERED),                  MP_OBJ_NEW_SMALL_INT(EVE_STATUS_REGISTERED) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_STATUS_ENABLE),                      MP_OBJ_NEW_SMALL_INT(EVE_STATUS_ENABLE) },
  { MP_OBJ_NEW_QSTR(MP_QSTR_EVE_STATUS_DISABLE),                     MP_OBJ_NEW_SMALL_INT(EVE_STATUS_DISABLE) },
};

static MP_DEFINE_CONST_DICT(mt_mpy_event_locals_dict, mt_mpy_event_locals_dict_table);

const mp_obj_type_t mt_mpy_event_type =
{
  { &mp_type_type },
  .name = MP_QSTR_event,
  .print = mt_mpy_event_print,
  .call = mt_mpy_event_call,
  .make_new = mt_mpy_event_make_new,
  .locals_dict = (mp_obj_t)&mt_mpy_event_locals_dict,
};

#else /* MODULE_event_ENABLE */ 

static const mp_map_elem_t mt_mpy_event_locals_dict_table[] =
{
};

static MP_DEFINE_CONST_DICT(mt_mpy_event_locals_dict, mt_mpy_event_locals_dict_table);

const mp_obj_type_t mt_mpy_event_type =
{
  { &mp_type_type },
  .name = MP_QSTR_event,
  .locals_dict = (mp_obj_t)&mt_mpy_event_locals_dict,
};

#endif /* MODULE_EVENT_ENABLE */ 