#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"

#include "bleprph.h"
//#include "message.h"
#include "drv_ble.h"
#include "mt_mpy_ble.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "mpy ble";
extern uint32_t ble_send_uplink(uint8_t *buf, uint8_t length);


/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/
typedef struct {
    mp_obj_base_t base;
    mp_obj_t callback;
}mpy_ble_obj_t;

STATIC void mpy_ble_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_printf(print, "%s\r\n", TAG);
};

STATIC  mpy_ble_obj_t mpy_ble_obj = { .base = {&mt_mpy_ble_type} };

STATIC mp_obj_t mpy_ble_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    // setup the object
    mpy_ble_obj_t *self = &mpy_ble_obj;
    self->base.type = &mt_mpy_ble_type;
    self->callback = mp_const_none;
    return self;
}



STATIC mp_obj_t mpy_ble_get_len(mp_obj_t self_in)
{
    uint32_t len;
    len = ble_get_msg_len();

    return mp_obj_new_int(len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(get_len_obj, mpy_ble_get_len);

static uint8_t buffer[256];
STATIC mp_obj_t mpy_ble_get_data(mp_obj_t self_in)
{
    uint32_t len;
    len = ble_get_msg(buffer);
    
    return mp_obj_new_bytes(buffer, len);

}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(get_data_obj, mpy_ble_get_data);

STATIC mp_obj_t mpy_ble_send(mp_obj_t self_in, mp_obj_t data)
{
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    ble_send_uplink(bufinfo.buf, bufinfo.len);
    return MP_OBJ_NEW_SMALL_INT(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(send_data_obj, mpy_ble_send);
STATIC mp_obj_t mpy_ble_invoke_irq(mp_int_t event)
{
    mpy_ble_obj_t *self = &mpy_ble_obj;

    ESP_LOGE(TAG, "invoke_irq function %p.\n", self->callback);
    if ((self->callback != mp_const_none) && (mp_obj_is_callable(self->callback))) {
        mp_sched_schedule(self->callback, MP_OBJ_FROM_PTR(mp_obj_new_int(event)));
    }
    return mp_const_none;

}


STATIC mp_obj_t mpy_ble_irq(mp_obj_t self_in, mp_obj_t handler_in)
{
    mpy_ble_obj_t *self = self_in;
    if (handler_in != mp_const_none && !mp_obj_is_callable(handler_in)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid handler"));
    }
    ESP_LOGE(TAG, "irq function %p.\n", handler_in);
    self->callback = handler_in;
    ble_irq_func_register(mpy_ble_invoke_irq);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(irq_obj, mpy_ble_irq);

STATIC mp_obj_t mpy_ble_set_protocol_version(mp_obj_t self_in, mp_obj_t protocol_ver)
{
    uint8_t protocol = mp_obj_get_int(protocol_ver);
    ble_set_protocol_version(protocol);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_protocol_version_obj, mpy_ble_set_protocol_version);

STATIC mp_obj_t mpy_ble_set_heart_enable(mp_obj_t self_in, mp_obj_t heart_en)
{
    uint8_t heart_enable = mp_obj_get_int(heart_en);
    ble_set_heart_enable(heart_enable);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_heart_enable_obj, mpy_ble_set_heart_enable);


STATIC mp_obj_t mpy_ble_get_ble_state(mp_obj_t self_in)
{
    uint8_t connect_state = ble_get_connect_state();
    uint8_t protocol = ble_get_protocol_version();
    uint8_t heart_enable = ble_get_heart_enable();
    mp_obj_t tuple[3] = {
        mp_obj_new_int(connect_state),
        mp_obj_new_int(protocol),
        mp_obj_new_int(heart_enable),
    };

    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(get_ble_state_obj, mpy_ble_get_ble_state);

STATIC mp_obj_t mpy_ble_get_ble_event(mp_obj_t self_in)
{
    uint16_t event = ble_get_event();
    return mp_obj_new_int(event);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(get_ble_event_obj, mpy_ble_get_ble_event);

STATIC mp_obj_t mpy_ble_connect_para_update(size_t n_args, const mp_obj_t *args)
{
    uint16_t latency = mp_obj_get_int(args[1]);
    uint16_t interval_min = mp_obj_get_int(args[2]);
    uint16_t interval_max = mp_obj_get_int(args[3]);
    uint16_t time_out = mp_obj_get_int(args[4]);

    ble_para_update(latency, interval_min, interval_max, time_out);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(connect_para_update_obj, 4, 5, mpy_ble_connect_para_update);


STATIC const mp_rom_map_elem_t mpy_ble_locals_dict_table[] =
{
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_len),                  (mp_obj_t)&get_len_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_data),                 (mp_obj_t)&get_data_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_data),                (mp_obj_t)&send_data_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_irq),                      (mp_obj_t)&irq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_protocol_version),     (mp_obj_t)&set_protocol_version_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_heart_enable),         (mp_obj_t)&set_heart_enable_obj },
        
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_ble_state),            (mp_obj_t)&get_ble_state_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_ble_event),            (mp_obj_t)&get_ble_event_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect_para_update),      (mp_obj_t)&connect_para_update_obj },
    
};

MP_DEFINE_CONST_DICT(mpy_ble_locals_dict, mpy_ble_locals_dict_table);

mp_obj_type_t mt_mpy_ble_type =
{
    { &mp_type_type },
    .name = MP_QSTR_ble,
    .print = mpy_ble_print,
    .make_new = mpy_ble_make_new,
    .locals_dict = (mp_obj_dict_t *)&mpy_ble_locals_dict,
};


