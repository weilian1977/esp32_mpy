#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/nlr.h"
#include "py/obj.h"

//#include "mt_err.h"
//#include "mt_log.h"
#include "mt_module_config.h"
#include "drv_button.h"
#include "mpy_button.h"

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
}mpy_button_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATAS
 ******************************************************************************/
static mpy_button_obj_t mpy_button_obj = {.base = {&mpy_button_type}};

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
static mp_obj_t mpy_button_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args)
{
  // parse args
  mp_map_t kw_args;
  mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
  
  // setup the object
  mpy_button_obj_t *self = &mpy_button_obj;
  self->base.type = &mpy_button_type;
  
  return self;
}

static mp_obj_t mpy_button_value(mp_obj_t self_in)
{
  uint8_t value = get_button_value();
  return mp_obj_new_int(value);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mpy_button_value_obj, mpy_button_value);

static mp_obj_t mpy_is_button_pressed(mp_obj_t self_in, mp_obj_t button)
{
  bool is_pressed = false;
  uint8_t button_id = mp_obj_get_int(button);
  is_pressed = is_key_pressed(button_id);
  return mp_obj_new_bool(is_pressed);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mpy_is_button_pressed_obj, mpy_is_button_pressed);

static mp_obj_t mpy_is_button_released(mp_obj_t self_in, mp_obj_t button)
{
  bool is_released = false;
  uint8_t button_id = mp_obj_get_int(button);
  is_released = is_key_released(button_id);
  return mp_obj_new_bool(is_released);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mpy_is_button_released_obj, mpy_is_button_released);

static void mpy_button_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{

}

static mp_obj_t mpy_button_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args)
{
  mp_arg_check_num(n_args, n_kw, 0, 0, false);
  
  return mp_const_none;
}

static const mp_map_elem_t mpy_button_locals_dict_table[] =
{
  { MP_OBJ_NEW_QSTR(MP_QSTR_value),               (mp_obj_t)&mpy_button_value_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_is_pressed),          (mp_obj_t)&mpy_is_button_pressed_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_is_released),         (mp_obj_t)&mpy_is_button_released_obj },
};

static MP_DEFINE_CONST_DICT(mpy_button_locals_dict, mpy_button_locals_dict_table);

const mp_obj_type_t mpy_button_type =
{
  { &mp_type_type },
  .name = MP_QSTR_button_board,
  .print = mpy_button_print,
  .call = mpy_button_call,
  .make_new = mpy_button_make_new,
  .locals_dict = (mp_obj_t)&mpy_button_locals_dict,
};

#else /* MODULE_BUTTON_ENABLE */

static const mp_map_elem_t mpy_button_locals_dict_table[] =
{

};

static MP_DEFINE_CONST_DICT(mpy_button_locals_dict, mpy_button_locals_dict_table);

const mp_obj_type_t mpy_button_type =
{
  { &mp_type_type },
  .name = MP_QSTR_button_board,
  .locals_dict = (mp_obj_t)&mpy_button_locals_dict,
};

#endif /* MODULE_BUTTON_ENABLE */