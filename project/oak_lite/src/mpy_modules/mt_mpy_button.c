#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/nlr.h"
#include "py/obj.h"

//#include "mt_err.h"
//#include "mt_log.h"
#include "module_config.h"
#include "mt_esp32_button.h"
#include "mt_mpy_button.h"

#if MODULE_BUTTON_ENABLE
/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define TAG                           ("mpy_button")

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/  
typedef struct
{
  mp_obj_base_t base;
}mt_mpy_button_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATAS
 ******************************************************************************/
static mt_mpy_button_obj_t mt_mpy_button_obj = {.base = {&mt_mpy_button_type}};

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
static mp_obj_t mt_mpy_button_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args)
{
  // parse args
  mp_map_t kw_args;
  mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
  
  // setup the object
  mt_mpy_button_obj_t *self = &mt_mpy_button_obj;
  self->base.type = &mt_mpy_button_type;
  
  return self;
}

static mp_obj_t mt_mpy_button_test(mp_obj_t self_in)
{
  mt_esp32_button_test_t();
  return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_button_test_obj, mt_mpy_button_test);

// static mp_obj_t mt_mpy_button_init(mp_obj_t self_in)
// {
//   mt_esp32_button_init_t();
//   return mp_const_true;
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_button_init_obj, mt_mpy_button_init);


static mp_obj_t mt_mpy_button_value(mp_obj_t self_in)
{
  uint8_t button_id = 0;
  uint8_t value = 0;

  mt_esp32_button_get_level_t(button_id, &value);

  return mp_obj_new_int(value);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_button_value_obj, mt_mpy_button_value);

static mp_obj_t mt_mpy_is_button_pressed(mp_obj_t self_in)
{
  uint8_t button_id = 0;
  bool is_pressed = false;

  mt_esp32_button_is_pressed_t(button_id, &is_pressed);

  return mp_obj_new_bool(is_pressed);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_is_button_pressed_obj, mt_mpy_is_button_pressed);

static mp_obj_t mt_mpy_is_button_released(mp_obj_t self_in)
{
  uint8_t button_id = 0;
  bool is_released = false;

  mt_esp32_button_is_released_t(button_id, &is_released);

  return mp_obj_new_bool(is_released);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_is_button_released_obj, mt_mpy_is_button_released);

static void mt_mpy_button_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{

}

static mp_obj_t mt_mpy_button_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args)
{
  mp_arg_check_num(n_args, n_kw, 0, 0, false);
  
  return mp_const_none;
}

static const mp_map_elem_t mt_mpy_button_locals_dict_table[] =
{
  { MP_OBJ_NEW_QSTR(MP_QSTR_value),               (mp_obj_t)&mt_mpy_button_value_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_is_pressed),          (mp_obj_t)&mt_mpy_is_button_pressed_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_is_released),         (mp_obj_t)&mt_mpy_is_button_released_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_test),                (mp_obj_t)&mt_mpy_button_test_obj },
};

static MP_DEFINE_CONST_DICT(mt_mpy_button_locals_dict, mt_mpy_button_locals_dict_table);

const mp_obj_type_t mt_mpy_button_type =
{
  { &mp_type_type },
  .name = MP_QSTR_button_board,
  .print = mt_mpy_button_print,
  .call = mt_mpy_button_call,
  .make_new = mt_mpy_button_make_new,
  .locals_dict = (mp_obj_t)&mt_mpy_button_locals_dict,
};

#else /* MODULE_BUTTON_ENABLE */

static const mp_map_elem_t mt_mpy_button_locals_dict_table[] =
{

};

static MP_DEFINE_CONST_DICT(mt_mpy_button_locals_dict, mt_mpy_button_locals_dict_table);

const mp_obj_type_t mt_mpy_button_type =
{
  { &mp_type_type },
  .name = MP_QSTR_button_board,
  .locals_dict = (mp_obj_t)&mt_mpy_button_locals_dict,
};

#endif /* MODULE_BUTTON_ENABLE */