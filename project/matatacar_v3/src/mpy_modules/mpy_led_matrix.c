

#include <stdint.h>
#include <stdio.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "drv_i2c.h"
#include "mpy_led_matrix.h"
#include "drv_aw20144.h"

#define   TAG                         ("mpy_led_matrix")
#if MODULE_LED_MATRIX_ENABLE
/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/

typedef struct {
    mp_obj_base_t base;
}mpy_led_matrix_obj_t;

STATIC void mpy_led_matrix_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_printf(print, "%s\r\n", TAG);
    mp_printf(print, "led_matrix print\r\n");
};

STATIC  mpy_led_matrix_obj_t mpy_led_matrix_obj = { .base = {&mpy_led_matrix_type} };

STATIC mp_obj_t mpy_led_matrix_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    // setup the object
    mpy_led_matrix_obj_t *self = &mpy_led_matrix_obj;
    self->base.type = &mpy_led_matrix_type;
    if(!is_aw20144_initialized())
    {
        aw20144_init();
    }
    return self;
}

STATIC mp_obj_t mpy_set_brightness(mp_obj_t self_in, mp_obj_t brightness)
{
    uint8_t brightness_data = mp_obj_get_int(brightness);
    aw20144_set_monochrome_leds_brightness(brightness_data);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_brightness_obj, mpy_set_brightness);

STATIC mp_obj_t mpy_clear_display(mp_obj_t self_in)
{
    aw20144_fast_clear_display();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(clear_display_obj, mpy_clear_display);

STATIC mp_obj_t mpy_show_image(mp_obj_t self_in, mp_obj_t arg)
{
    uint8_t *pbuf = NULL;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    pbuf = (uint8_t *)bufinfo.buf;
    aw20144_show_image(pbuf, bufinfo.len);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(show_image_obj, mpy_show_image);

STATIC mp_obj_t mpy_set_pixel(size_t n_args, const mp_obj_t *args)
{
    int16_t x = mp_obj_get_int(args[1]);
    int16_t y = mp_obj_get_int(args[2]);
    uint8_t brightness = mp_obj_get_int(args[3]);
    aw20144_set_pixel(x, y, brightness);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(set_pixel_obj, 4, 4, mpy_set_pixel);

STATIC const mp_rom_map_elem_t mpy_led_matrix_locals_dict_table[] = 
{
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_brightness),    (mp_obj_t)&set_brightness_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear),             (mp_obj_t)&clear_display_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_show_image),        (mp_obj_t)&show_image_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_pixel),         (mp_obj_t)&set_pixel_obj},
};

MP_DEFINE_CONST_DICT(mpy_led_matrix_locals_dict, mpy_led_matrix_locals_dict_table);

const mp_obj_type_t mpy_led_matrix_type =
{
    { &mp_type_type },
    .name = MP_QSTR_led_matrix,
    .print = mpy_led_matrix_print,
    .make_new = mpy_led_matrix_make_new,
    .locals_dict = (mp_obj_dict_t *)&mpy_led_matrix_locals_dict,
};
#else /* MODULE_LED_MATRIX_ENABLE */
static const mp_map_elem_t mpy_led_matrix_locals_dict_table[] =
{
};

static MP_DEFINE_CONST_DICT(mpy_led_matrix_locals_dict, mpy_led_matrix_locals_dict_table);

const mp_obj_type_t mpy_led_matrix_type =
{
    { &mp_type_type },
    .name = MP_QSTR_led_matrix,
    .locals_dict = (mp_obj_t)&mpy_led_matrix_locals_dict,
};

#endif /* MODULE_LED_MATRIX_ENABLE */
