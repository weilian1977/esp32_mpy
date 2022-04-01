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
#include "mt_light_sensor.h"
#include "mt_mpy_light_sensor.h"

#if MODULE_LIGHT_SENSOR_ENABLE
/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define TAG                           ("mpy_light_sensor")

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/  
typedef struct
{
  mp_obj_base_t base;
}mt_mpy_light_sensor_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATAS
 ******************************************************************************/
static mt_mpy_light_sensor_obj_t mt_mpy_light_sensor_obj = {.base = {&mt_mpy_light_sensor_type}};

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
static mp_obj_t mt_mpy_light_sensor_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args)
{
  // parse args
  mp_map_t kw_args;
  mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
  
  // setup the object
  mt_mpy_light_sensor_obj_t *self = &mt_mpy_light_sensor_obj;
  self->base.type = &mt_mpy_light_sensor_type;
  
  return self;
}

static mp_obj_t mt_mpy_light_sensor_get_value(mp_obj_t self_in,  mp_obj_t arg1)
{
  int value = 0;  
  uint8_t axis;

  mt_light_sensor_get_value_t(&value);

  return mp_obj_new_int(value);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mt_mpy_light_sensor_get_value_obj, mt_mpy_light_sensor_get_value);

static void mt_mpy_light_sensor_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{

}

static mp_obj_t mt_mpy_light_sensor_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args)
{
  mp_arg_check_num(n_args, n_kw, 0, 0, false);
  
  return mp_const_none;
}

static const mp_map_elem_t mt_mpy_light_sensor_locals_dict_table[] =
{
  { MP_OBJ_NEW_QSTR(MP_QSTR_get_value),        (mp_obj_t)&mt_mpy_light_sensor_get_value_obj },
};

static MP_DEFINE_CONST_DICT(mt_mpy_light_sensor_locals_dict, mt_mpy_light_sensor_locals_dict_table);

const mp_obj_type_t mt_mpy_light_sensor_type =
{
  { &mp_type_type },
  .name = MP_QSTR_light_sensor_board,
  .print = mt_mpy_light_sensor_print,
  .call = mt_mpy_light_sensor_call,
  .make_new = mt_mpy_light_sensor_make_new,
  .locals_dict = (mp_obj_t)&mt_mpy_light_sensor_locals_dict,
};

#endif /* MODULE_LIGHT_SENSOR_ENABLE */