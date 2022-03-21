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
#include "drv_qmi8658.h"
#include "mt_mpy_gyro.h"

#if MODULE_GYRO_ENABLE
/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define TAG                           ("mpy_gyro")

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/  
typedef struct
{
  mp_obj_base_t base;
}mt_mpy_gyro_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATAS
 ******************************************************************************/
static mt_mpy_gyro_obj_t mt_mpy_gyro_obj = {.base = {&mt_mpy_gyro_type}};

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
static mp_obj_t mt_mpy_gyro_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args)
{
  // parse args
  mp_map_t kw_args;
  mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
  
  // setup the object
  mt_mpy_gyro_obj_t *self = &mt_mpy_gyro_obj;
  self->base.type = &mt_mpy_gyro_type;
  
  return self;
}

// static mp_obj_t mt_mpy_gyro_init(mp_obj_t self_in)
// {
//   mt_esp32_gyro_init_t();
//   return mp_const_true;
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_gyro_init_obj, mt_mpy_gyro_init);

static mp_obj_t mt_mpy_gyro_get_acc(mp_obj_t self_in,  mp_obj_t arg1)
{
  float value = 0;  
  uint8_t axis;

  axis = mp_obj_get_int(arg1);

  mt_esp32_gyro_get_acc_t(&value, axis);

  return mp_obj_new_float(value);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_gyro_get_acc_obj, mt_mpy_gyro_get_acc);

static mp_obj_t mt_mpy_gyro_get_gyro(mp_obj_t self_in,  mp_obj_t arg1)
{
  float value = 0;  
  uint8_t axis;

  axis = mp_obj_get_int(arg1);

  mt_esp32_gyro_get_gyro_t(&value, axis);

  return mp_obj_new_float(value);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_gyro_get_gyro_obj, mt_mpy_gyro_get_gyro);

static mp_obj_t mt_mpy_gyro_get_tilt_status(mp_obj_t self_in, mp_obj_t arg1)
{
  uint16_t ret_status = 0;
  uint16_t id = mp_obj_get_int(arg1);

  mt_esp32_gyro_get_tilt_status_t(id, &ret_status);
  return mp_obj_new_int(ret_status);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mt_mpy_gyro_get_tilt_status_obj, mt_mpy_gyro_get_tilt_status);

static void mt_mpy_gyro_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{

}

static mp_obj_t mt_mpy_gyro_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args)
{
  mp_arg_check_num(n_args, n_kw, 0, 0, false);
  
  return mp_const_none;
}

static const mp_map_elem_t mt_mpy_gyro_locals_dict_table[] =
{
  //{ MP_OBJ_NEW_QSTR(MP_QSTR_get_angle),           (mp_obj_t)&mt_mpy_gyro_get_angle_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_get_acc),             (mp_obj_t)&mt_mpy_gyro_get_acc_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_get_gyro),            (mp_obj_t)&mt_mpy_gyro_get_gyro_obj },
  //{ MP_OBJ_NEW_QSTR(MP_QSTR_get_rotate_angle),    (mp_obj_t)&mt_mpy_gyro_get_rotate_angle_obj },
  //{ MP_OBJ_NEW_QSTR(MP_QSTR_reset_rotate_angle),  (mp_obj_t)&mt_mpy_gyro_reset_rotate_angle_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_get_tilt),            (mp_obj_t)&mt_mpy_gyro_get_tilt_status_obj },
  //{ MP_OBJ_NEW_QSTR(MP_QSTR_set_shake_threshold), (mp_obj_t)&mt_mpy_gyro_set_shaked_threshold_obj },
  //{ MP_OBJ_NEW_QSTR(MP_QSTR_get_shaked_strength), (mp_obj_t)&mt_mpy_gyro_get_shake_strengh_obj },
  //{ MP_OBJ_NEW_QSTR(MP_QSTR_is_shaked),           (mp_obj_t)&mt_mpy_gyro_is_shaked_obj },
  //{ MP_OBJ_NEW_QSTR(MP_QSTR_test),                (mp_obj_t)&mt_mpy_gyro_test_obj },
};

static MP_DEFINE_CONST_DICT(mt_mpy_gyro_locals_dict, mt_mpy_gyro_locals_dict_table);

const mp_obj_type_t mt_mpy_gyro_type =
{
  { &mp_type_type },
  .name = MP_QSTR_gyro_board,
  .print = mt_mpy_gyro_print,
  .call = mt_mpy_gyro_call,
  .make_new = mt_mpy_gyro_make_new,
  .locals_dict = (mp_obj_t)&mt_mpy_gyro_locals_dict,
};

#else /* MODULE_GYRO_ENABLE */

static const mp_map_elem_t mt_mpy_gyro_locals_dict_table[] =
{

};

static MP_DEFINE_CONST_DICT(mt_mpy_gyro_locals_dict, mt_mpy_gyro_locals_dict_table);

const mp_obj_type_t mt_mpy_gyro_type =
{
  { &mp_type_type },
  .name = MP_QSTR_gyro_board,
  .locals_dict = (mp_obj_t)&mt_mpy_gyro_locals_dict,
};

#endif /* MODULE_GYRO_ENABLE */