#include <stdio.h>
#include <string.h>
#include <math.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "drv_light_sensor.h"
#include "drv_infrared_tube.h"
#include "mpy_sensor.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "mpy sensor";

#if MODULE_SENSOR_ENABLE
/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/
typedef struct {
    mp_obj_base_t base;
}mpy_sensor_obj_t;

static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

STATIC void mpy_sensor_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_printf(print, "%s\r\n", TAG);
    mp_printf(print, "sensor print\r\n");
};

STATIC  mpy_sensor_obj_t mpy_sensor_obj = { .base = {&mpy_sensor_type} };

STATIC mp_obj_t mpy_sensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    // setup the object
    mpy_sensor_obj_t *self = &mpy_sensor_obj;
    self->base.type = &mpy_sensor_type;
    return self;
}

STATIC mp_obj_t mpy_get_light_value(mp_obj_t self_in, mp_obj_t light_channel)
{
    uint8_t channel = mp_obj_get_int(light_channel);
    float light_value = get_light_value(channel);
    return mp_obj_new_float(light_value);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(get_light_value_obj, mpy_get_light_value);

STATIC mp_obj_t mpy_get_light_intensity(mp_obj_t self_in, mp_obj_t light_channel)
{
    uint8_t channel = mp_obj_get_int(light_channel);
    float light_value = get_light_value(channel);
    int16_t light_intensity = (int16_t)(map(light_value, 0, 3.3, 0, 100));
    return mp_obj_new_int(light_intensity);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(get_light_intensity_obj, mpy_get_light_intensity);

STATIC mp_obj_t mpy_get_obstacle_avoidance_value(mp_obj_t self_in)
{
    float ir_value = get_infrared_tube_value(0);
    return mp_obj_new_float(ir_value);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(get_obstacle_avoidance_value_obj, mpy_get_obstacle_avoidance_value);

STATIC mp_obj_t mpy_get_line_follower_1_value(mp_obj_t self_in)
{
    float ir_value = get_infrared_tube_value(1);
    return mp_obj_new_float(ir_value);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(get_line_follower_1_value_obj, mpy_get_line_follower_1_value);


STATIC mp_obj_t mpy_get_line_follower_2_value(mp_obj_t self_in)
{
    float ir_value = get_infrared_tube_value(2);
    return mp_obj_new_float(ir_value);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(get_line_follower_2_value_obj, mpy_get_line_follower_2_value);


STATIC const mp_rom_map_elem_t mpy_sensor_locals_dict_table[] =
{
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_light_value),                (mp_obj_t)&get_light_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_light_intensity),            (mp_obj_t)&get_light_intensity_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_obstacle_avoidance_value),   (mp_obj_t)&get_obstacle_avoidance_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_line_follower_1_value),      (mp_obj_t)&get_line_follower_1_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_line_follower_2_value),      (mp_obj_t)&get_line_follower_2_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LEFT),                           MP_OBJ_NEW_SMALL_INT(0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RIGHT),                          MP_OBJ_NEW_SMALL_INT(1) },
};

MP_DEFINE_CONST_DICT(mpy_sensor_locals_dict, mpy_sensor_locals_dict_table);

mp_obj_type_t mpy_sensor_type =
{
    { &mp_type_type },
    .name = MP_QSTR_sensor,
    .print = mpy_sensor_print,
    .make_new = mpy_sensor_make_new,
    .locals_dict = (mp_obj_dict_t *)&mpy_sensor_locals_dict,
};
#else /* MODULE_SENSOR_ENABLE */
static const mp_map_elem_t mpy_sensor_locals_dict_table[] =
{
};

static MP_DEFINE_CONST_DICT(mpy_sensor_locals_dict, mpy_sensor_locals_dict_table);

const mp_obj_type_t mpy_sensor_type =
{
    { &mp_type_type },
    .name = MP_QSTR_sensor,
    .locals_dict = (mp_obj_t)&mpy_sensor_locals_dict,
};
#endif /* MODULE_SENSOR_ENABLE */
