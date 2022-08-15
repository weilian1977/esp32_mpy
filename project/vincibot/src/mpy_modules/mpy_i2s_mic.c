#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/nlr.h"
#include "py/obj.h"

#include "esp_err.h"
#include "esp_log.h"
//#include "module_config.h"
#include "i2s_mic.h"
#include "mpy_i2s_mic.h"
#define MODULE_I2S_MIC_ENABLE 1
#if MODULE_I2S_MIC_ENABLE
/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define  TAG      ("mpy_i2s_mic")

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/
typedef struct
{
    mp_obj_base_t base;
}mpy_i2s_mic_obj_t;

/******************************************************************************
 DEFINE PRIVATE DATAS
 ******************************************************************************/
static mpy_i2s_mic_obj_t mpy_i2s_mic_obj = {.base = {&mpy_i2s_mic_type}};

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
static mp_obj_t mpy_i2s_mic_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args)
{
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    // setup the object
    mpy_i2s_mic_obj_t *self = &mpy_i2s_mic_obj;
    self->base.type = &mpy_i2s_mic_type;
    return self;
}

static mp_obj_t mpy_i2s_mic_init(mp_obj_t self_in)
{
    return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mpy_i2s_mic_init_obj, mpy_i2s_mic_init);

static mp_obj_t mpy_i2s_mic_deinit(mp_obj_t self_in)
{ 
    return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mpy_i2s_mic_deinit_obj, mpy_i2s_mic_deinit);

static mp_obj_t mpy_i2s_mic_get_loudness(mp_obj_t self_in, mp_obj_t arg1)
{
    size_t str_len;
    float volume_value;
    const char *type = mp_obj_str_get_data(arg1, &str_len);
    if(strcmp(type, "average") == 0)
    {
        get_loudness_value(average_value_type, &volume_value);
    }
    else
    {
        get_loudness_value(maximum_value_type, &volume_value);
    }
    return mp_obj_new_int(volume_value);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mpy_i2s_mic_get_loudness_obj, mpy_i2s_mic_get_loudness);

static void mpy_i2s_mic_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{

}

static mp_obj_t mpy_i2s_mic_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return mp_const_none;
}

static const mp_map_elem_t mpy_i2s_mic_locals_dict_table[] =
{
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),                        (mp_obj_t)&mpy_i2s_mic_init_obj },        
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),                      (mp_obj_t)&mpy_i2s_mic_deinit_obj },    
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_loudness),                (mp_obj_t)&mpy_i2s_mic_get_loudness_obj },
};

static MP_DEFINE_CONST_DICT(mpy_i2s_mic_locals_dict, mpy_i2s_mic_locals_dict_table);

const mp_obj_type_t mpy_i2s_mic_type =
{
    { &mp_type_type },
    .name = MP_QSTR_i2s_mic,
    .print = mpy_i2s_mic_print,
    .call = mpy_i2s_mic_call,
    .make_new = mpy_i2s_mic_make_new,
    .locals_dict = (mp_obj_t)&mpy_i2s_mic_locals_dict,
};

#else /* MODULE_I2S_MIC_ENABLE */
static const mp_map_elem_t mpy_i2s_mic_locals_dict_table[] =
{
};

static MP_DEFINE_CONST_DICT(mpy_i2s_mic_locals_dict, mpy_i2s_mic_locals_dict_table);

const mp_obj_type_t mpy_i2s_mic_type =
{
    { &mp_type_type },
    .name = MP_QSTR_i2s_mic,
    .locals_dict = (mp_obj_t)&mpy_i2s_mic_locals_dict,
};

#endif /* MODULE_I2S_MIC_ENABLE */