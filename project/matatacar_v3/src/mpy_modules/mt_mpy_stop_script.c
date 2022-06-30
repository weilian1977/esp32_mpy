#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/mphal.h"
#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/mphal.h"

#include "mt_log.h"
#include "mt_module_config.h"
#include "mt_stop_python_thread.h"
#include "mt_mpy_stop_script.h"


#define TAG                                  ("mt_mpy_stop_script")

#if MODULE_STOP_PYTHON_THREAD_ENABLE
/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define STOP_SCRIPT_WAIT_MAX_TIEM_MS (2000)

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/  
typedef struct
{
  mp_obj_base_t base;
}mt_mpy_stop_thread_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATAS
 ******************************************************************************/
static mt_mpy_stop_thread_obj_t mt_mpy_stop_thread_obj = {.base = {&mt_mpy_stop_thread_type }};

/******************************************************************************
 binding to micropython
 ******************************************************************************/
static mp_obj_t mt_mpy_stop_thread_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args)
{
  // parse args
  mp_map_t kw_args;
  mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
  
  // setup the object
  mt_mpy_stop_thread_obj_t *self = &mt_mpy_stop_thread_obj;
  self->base.type = &mt_mpy_stop_thread_type;
  return self;
}

// static mp_obj_t mt_mpy_stop_thread_test(mp_obj_t self_in)
// {
//   return mp_const_none;
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_stop_thread_test_obj, mt_mpy_stop_thread_test);

static mp_obj_t mt_mpy_get_current_handle_and_priority(mp_obj_t self_in)
{
  void *handle = NULL;
  int priority = 0;
  mt_thread_get_current_handle_and_priority_t(&handle, &priority);
  
  mp_obj_t ret_list = mp_obj_new_list(0, NULL);
  mp_obj_list_append(ret_list, mp_obj_new_int((int)(handle)));
  mp_obj_list_append(ret_list, mp_obj_new_int(priority));

  return ret_list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_get_current_handle_and_priority_obj, mt_mpy_get_current_handle_and_priority);

static mp_obj_t mt_mpy_add_thread_to_table(mp_obj_t self_in)
{
  int8_t thread_id =-1;
  mt_thread_add_thread_to_table_t(NULL, &thread_id);
  
  return mp_obj_new_int(thread_id);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_add_thread_to_table_obj, mt_mpy_add_thread_to_table);

static mp_obj_t mt_mpy_remove_thread_from_table_by_id(mp_obj_t self_in, mp_obj_t arg1)
{
  int8_t id = mp_obj_get_int(arg1);
  if(mt_thread_table_remove_item_by_id_t(id) == 0)
  {
    return mp_const_true;
  }
  else
  {
    return mp_const_false;
  }
  
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_remove_thread_from_table_by_id_obj, mt_mpy_remove_thread_from_table_by_id);

static mp_obj_t mt_mpy_remove_thread_from_table_by_handle(mp_obj_t self_in, mp_obj_t arg1)
{
  uint32_t han = mp_obj_get_int(arg1);
  if(mt_thread_table_remove_item_by_handle_t((TaskHandle_t)han) == 0)
  {
    return mp_const_true;
  }
  else
  {
    return mp_const_false;
  }
  
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_remove_thread_from_table_by_handle_obj, mt_mpy_remove_thread_from_table_by_handle);

static mp_obj_t mt_mpy_set_thread_status_by_id(mp_obj_t self_in, mp_obj_t arg1, mp_obj_t arg2)
{
  uint8_t id = mp_obj_get_int(arg1);
  uint8_t sta = mp_obj_get_int(arg2);
  mt_thread_set_status_by_id_t(id, sta);
  ESP_LOGI(TAG, "set_status thread id is %d, status is %d", id, sta);
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(mt_mpy_set_thread_status_by_id_obj, mt_mpy_set_thread_status_by_id);

static mp_obj_t mt_mpy_get_thread_status_by_id(mp_obj_t self_in, mp_obj_t arg1)
{ 
  uint8_t id = mp_obj_get_int(arg1);
  int8_t sta = 0;
  mt_thread_get_status_by_id_t(id, &sta);
  ESP_LOGI(TAG, "get_status thread id is %d, status is %d", id, sta);
  return mp_obj_new_int(sta);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_get_thread_status_by_id_obj, mt_mpy_get_thread_status_by_id);

static mp_obj_t mp_mpy_stop_other_script(mp_obj_t self_in)
{ 
  int8_t status = -1;
  mt_thread_get_module_status_t(&status);
  ESP_LOGI(TAG, "mt_thread_get_module_status_t is %d", status);
  if(status == MP_THREAD_SCHEDULE_RESTARTING)
  {
    return mp_const_false;
  }
  ESP_LOGI(TAG, "mt_thread_stop_other_script_t");
  mt_thread_stop_other_script_t();
  mt_thread_stop_scripts_wait_done_t(STOP_SCRIPT_WAIT_MAX_TIEM_MS);
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_mpy_stop_other_script_obj, mp_mpy_stop_other_script);

static mp_obj_t mp_mpy_stop_self_script(mp_obj_t self_in)
{ 
  int8_t status = -1;
  mt_thread_get_module_status_t(&status);
  if(status == MP_THREAD_SCHEDULE_RESTARTING)
  {
    return mp_const_false;
  }
  mt_thread_stop_self_t();
  
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_mpy_stop_self_script_obj, mp_mpy_stop_self_script);

static mp_obj_t mp_mpy_stop_all_script(mp_obj_t self_in)
{ 
  int8_t status = -1;
  mt_thread_get_module_status_t(&status);
  if(status == MP_THREAD_SCHEDULE_RESTARTING)
  {
    return mp_const_false;
  }

  mt_thread_stop_all_script_t();
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_mpy_stop_all_script_obj, mp_mpy_stop_all_script);

static const mp_map_elem_t mt_mpy_stop_thread_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_priority),              (mp_obj_t)(&mt_mpy_get_current_handle_and_priority_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_add_thread),                (mp_obj_t)(&mt_mpy_add_thread_to_table_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove_thread_by_id),       (mp_obj_t)(&mt_mpy_remove_thread_from_table_by_id_obj) },  
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove_thread_by_handle),   (mp_obj_t)(&mt_mpy_remove_thread_from_table_by_handle_obj) },  

    { MP_OBJ_NEW_QSTR(MP_QSTR_stop_other_script),         (mp_obj_t)(&mp_mpy_stop_other_script_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stop_this_script),          (mp_obj_t)(&mp_mpy_stop_self_script_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stop_all_script),           (mp_obj_t)(&mp_mpy_stop_all_script_obj) },   
    
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_thread_sta),            (mp_obj_t)(&mt_mpy_set_thread_status_by_id_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_thread_sta),            (mp_obj_t)(&mt_mpy_get_thread_status_by_id_obj) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_THREAD_MAX),                MP_OBJ_NEW_SMALL_INT(MP_THREAD_MAX)},

    { MP_OBJ_NEW_QSTR(MP_QSTR_THREAD_RESTARTED),          MP_OBJ_NEW_SMALL_INT(MP_THREAD_RESTARTED)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_THREAD_EXECUTING),          MP_OBJ_NEW_SMALL_INT(MP_THREAD_EXECUTING)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_THREAD_WAIT_TO_RESTART),    MP_OBJ_NEW_SMALL_INT(MP_THREAD_WAIT_TO_RESTART)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_THREAD_FATAL_ERROR),        MP_OBJ_NEW_SMALL_INT(MP_THREAD_FATAL_ERROR)},

    { MP_OBJ_NEW_QSTR(MP_QSTR_SCHEDULE_ON_START),          MP_OBJ_NEW_SMALL_INT(MP_THREAD_SCHEDULE_ON_START)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCHEDULE_RESTARTING),        MP_OBJ_NEW_SMALL_INT(MP_THREAD_SCHEDULE_RESTARTING)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCHEDULE_RESTARTED),         MP_OBJ_NEW_SMALL_INT(MP_THREAD_SCHEDULE_RESTARTED)},

};
static MP_DEFINE_CONST_DICT(mt_mpy_stop_thread_locals_dict, mt_mpy_stop_thread_locals_dict_table);

const mp_obj_type_t mt_mpy_stop_thread_type =
{
  { &mp_type_type },
  .name = MP_QSTR_stop_thread,
  .make_new = mt_mpy_stop_thread_make_new,
  .locals_dict = (mp_obj_t)&mt_mpy_stop_thread_locals_dict,
};

#else /* MODULE_STOP_PYTHON_THREAD_ENABLE */

static const mp_map_elem_t mt_mpy_stop_thread_locals_dict_table[] = {

};
static MP_DEFINE_CONST_DICT(mt_mpy_stop_thread_locals_dict, mt_mpy_stop_thread_locals_dict_table);

const mp_obj_type_t mt_mpy_stop_thread_type =
{
  { &mp_type_type },
  .name = MP_QSTR_stop_thread,
  .locals_dict = (mp_obj_t)&mt_mpy_stop_thread_locals_dict,
};

#endif /* MODULE_STOP_PYTHON_THREAD_ENABLE */