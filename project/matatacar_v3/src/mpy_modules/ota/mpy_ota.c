#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/nlr.h"
#include "py/obj.h"

#include "esp_err.h"
#include "esp_log.h"
#include "drv_ota.h"
#include "mpy_ota.h"

/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define   TAG                         ("mpy_ota")

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/
typedef struct
{
  mp_obj_base_t base;
}mpy_ota_obj_t;

/******************************************************************************
 DEFINE PRIVATE DATAS
 ******************************************************************************/
STATIC mpy_ota_obj_t mpy_ota_obj = {.base = {&mpy_ota_type}};

/******************************************************************************
 DEFINE PUBLIC DATAS
 ******************************************************************************/

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
 
/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 DEFINE MICROPYTHON FUNCTIONS
 ******************************************************************************/
STATIC mp_obj_t mpy_ota_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args)
{
  // parse args
  mp_map_t kw_args;
  mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
  // setup the object
  mpy_ota_obj_t *self = &mpy_ota_obj;
  self->base.type = &mpy_ota_type;
  return self;
}

STATIC mp_obj_t mpy_ota_set_wifi_info(mp_obj_t self_in, mp_obj_t arg1, mp_obj_t arg2)
{
  int32_t item = mp_obj_get_int(arg1);

  if(MP_OBJ_IS_TYPE(arg2, &mp_type_bytearray))
  {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg2, &bufinfo, MP_BUFFER_READ);

    char *str_buf;
    str_buf = (char *)malloc(bufinfo.len);
    memcpy(&str_buf[0], bufinfo.buf, bufinfo.len);

    MP_THREAD_GIL_EXIT();
    esp32_ota_config_set_wifi_info(item, str_buf);
    free(str_buf);
    MP_THREAD_GIL_ENTER();
  }
  else
  {
    return mp_const_none;
  }
  /* arg2 : data list */
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mpy_ota_set_wifi_info_obj, mpy_ota_set_wifi_info);

STATIC mp_obj_t mpy_ota_get_wifi_info(mp_obj_t self_in, mp_obj_t arg1)
{
  int32_t item = mp_obj_get_int(arg1);
  char str_buf[128];

  MP_THREAD_GIL_EXIT();
  esp32_ota_config_get_wifi_info(item, str_buf);
  MP_THREAD_GIL_ENTER();
  
  return mp_obj_new_str(str_buf, strlen(str_buf));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mpy_ota_get_wifi_info_obj, mpy_ota_get_wifi_info);

STATIC mp_obj_t mpy_ota_start(mp_obj_t self_in)//mb_esp32_ota_start_t
{ 
  printf("try to run into ota update codes.\n");

  esp32_ota_start();

  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_ota_start_obj, mpy_ota_start);

STATIC void mpy_ota_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_printf(print, "%s\r\n", TAG);
}

STATIC mp_obj_t mpy_ota_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args)
{
  mp_arg_check_num(n_args, n_kw, 0, 0, false);
  return mp_const_none;
}

STATIC const mp_rom_map_elem_t mpy_ota_locals_dict_table[] =
{
  { MP_OBJ_NEW_QSTR(MP_QSTR_ota_set_wifi_info),           (mp_obj_t)&mpy_ota_set_wifi_info_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_ota_get_wifi_info),           (mp_obj_t)&mpy_ota_get_wifi_info_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_ota_start),                   (mp_obj_t)&mpy_ota_start_obj },
};

MP_DEFINE_CONST_DICT(mpy_ota_locals_dict, mpy_ota_locals_dict_table);

mp_obj_type_t mpy_ota_type =
{
  { &mp_type_type },
  .name = MP_QSTR_ota,
  .print = mpy_ota_print,
  .call = mpy_ota_call,
  .make_new = mpy_ota_make_new,
  .locals_dict = (mp_obj_t)&mpy_ota_locals_dict,
};

