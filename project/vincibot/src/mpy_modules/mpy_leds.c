

#include <stdint.h>
#include <stdio.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "drv_i2c.h"
#include "mpy_leds.h"
#include "drv_aw20144.h"

#define   TAG                         ("mpy_leds")
#if MODULE_LEDS_ENABLE
/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/

typedef struct {
    mp_obj_base_t base;
}mpy_leds_obj_t;

STATIC void mpy_leds_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_printf(print, "%s\r\n", TAG);
    mp_printf(print, "leds print\r\n");
};

STATIC  mpy_leds_obj_t mpy_leds_obj = { .base = {&mpy_leds_type} };

STATIC mp_obj_t mpy_leds_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    // setup the object
    mpy_leds_obj_t *self = &mpy_leds_obj;
    self->base.type = &mpy_leds_type;
    if(!is_aw20144_initialized())
    {
        aw20144_init();
    }
    return self;
}

STATIC mp_obj_t mpy_show_single(size_t n_args, const mp_obj_t *args)
{
    int16_t mode = mp_obj_get_int(args[1]);
    int16_t red = mp_obj_get_int(args[2]);
    int16_t green = mp_obj_get_int(args[3]);
    int16_t blue = mp_obj_get_int(args[4]);
    if(mode == LED_RIGHT)
    {
        set_right_rgb_led(red, green, blue);
    }
    else if(mode == LED_LEFT)
    {
        set_left_rgb_led(red, green, blue);
    }
    else
    {
        set_right_rgb_led(red, green, blue);
        set_left_rgb_led(red, green, blue);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(show_single_obj, 5, 5, mpy_show_single);

STATIC mp_obj_t mpy_show_all(size_t n_args, const mp_obj_t *args)
{
    int16_t red = mp_obj_get_int(args[1]);
    int16_t green = mp_obj_get_int(args[2]);
    int16_t blue = mp_obj_get_int(args[3]);

    set_right_rgb_led(red, green, blue);
    set_left_rgb_led(red, green, blue);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(show_all_obj, 4, 4, mpy_show_all);

STATIC const mp_rom_map_elem_t mpy_leds_locals_dict_table[] = 
{
    { MP_OBJ_NEW_QSTR(MP_QSTR_show_single), (mp_obj_t)&show_single_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_show_all), (mp_obj_t)&show_all_obj},
};

MP_DEFINE_CONST_DICT(mpy_leds_locals_dict, mpy_leds_locals_dict_table);

const mp_obj_type_t mpy_leds_type =
{
    { &mp_type_type },
    .name = MP_QSTR_leds,
    .print = mpy_leds_print,
    .make_new = mpy_leds_make_new,
    .locals_dict = (mp_obj_dict_t *)&mpy_leds_locals_dict,
};
#else /* MODULE_LEDS_ENABLE */
static const mp_map_elem_t mpy_leds_locals_dict_table[] =
{
};

static MP_DEFINE_CONST_DICT(mpy_leds_locals_dict, mpy_leds_locals_dict_table);

const mp_obj_type_t mpy_leds_type =
{
    { &mp_type_type },
    .name = MP_QSTR_leds,
    .locals_dict = (mp_obj_t)&mpy_leds_locals_dict,
};

#endif /* MODULE_LEDS_ENABLE */
