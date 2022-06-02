#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "py/mphal.h"
#include "esp_system.h"
#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "uart.h"

#include "mt_err.h"
#include "mt_module_config.h"

#include "mt_mpy_event.h"
#include "mpy_motion.h"
#include "mpy_sensor.h"
#include "drv_nvs.h"
#include "mpy_leds.h"
#include "mpy_led_matrix.h"
//#include "mt_mpy_stop_script.h"
#include "mpy_button.h"
#include "mt_mpy_ble.h"
#include "battery_check.h"
#include "usb_detect.h"
#include "indicator_led.h"
#include "power_management.h"
#include "firmware_version.h"

#define   TAG                         ("matatalab")

#define MODULE_ENABLE_VALUE   (1)
#define MODULE_DISABLE_VALUE  (0)

//extern void mp_task_reset(void);
STATIC mp_obj_t mpy_indicator_led(mp_obj_t led_status)
{
    int16_t led_value = mp_obj_get_int(led_status);
    indicator_led_set(led_value);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(indicator_led_obj, mpy_indicator_led);

STATIC mp_obj_t mpy_is_usb_detected(void)
{
    bool usb_detected = false;
    usb_detected = is_usb_detected();
    if(usb_detected == true)
    {
        return mp_const_true;
    }
    else
    {
        return mp_const_false;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(is_usb_detected_obj, mpy_is_usb_detected);

STATIC mp_obj_t mpy_get_battery_voltage(void)
{
    float battery_voltage = 0.0f;
    battery_voltage = get_battery_voltage();
    return mp_obj_new_float(battery_voltage);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_battery_voltage_obj, mpy_get_battery_voltage);

STATIC mp_obj_t mpy_get_battery_capacity(void)
{
    uint8_t battery_capacity = 0;
    battery_capacity = get_battery_capacity();
    return mp_obj_new_int(battery_capacity);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_battery_capacity_obj, mpy_get_battery_capacity);

STATIC mp_obj_t mpy_power_off(void)
{
    power_off();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(power_off_obj, mpy_power_off);

STATIC mp_obj_t mpy_power_on(void)
{
    power_on();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(power_on_obj, mpy_power_on);

STATIC mp_obj_t mpy_get_power_state(void)
{
    bool power_state = false;
    power_state = get_power_pin_state();
    return mp_obj_new_bool(power_state);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_power_state_obj, mpy_get_power_state);

// STATIC mp_obj_t mpy_reset(void)
// {
//     mp_task_reset();
//     return mp_const_none;
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_reset_obj, mpy_reset);

STATIC mp_obj_t mpy_nvs_write_string(mp_uint_t n_args, const mp_obj_t *args)
{
    mp_buffer_info_t bufinfo_1;
    mp_get_buffer_raise(args[0], &bufinfo_1, MP_BUFFER_READ);
    char *str_buf_1;
    str_buf_1 = (char *)malloc(bufinfo_1.len + 1);
    memcpy(&str_buf_1[0], bufinfo_1.buf, bufinfo_1.len);
    memcpy(&str_buf_1[bufinfo_1.len], "\0", 1);

    mp_buffer_info_t bufinfo_2;
    mp_get_buffer_raise(args[1], &bufinfo_2, MP_BUFFER_READ);
    char *str_buf_2;
    str_buf_2 = (char *)malloc(bufinfo_2.len + 1);
    memcpy(&str_buf_2[0], bufinfo_2.buf, bufinfo_2.len);
    memcpy(&str_buf_2[bufinfo_2.len], "\0", 1);

    mp_buffer_info_t bufinfo_3;
    mp_get_buffer_raise(args[2], &bufinfo_3, MP_BUFFER_READ);
    char *str_buf_3;
    str_buf_3 = (char *)malloc(bufinfo_3.len + 1);
    memcpy(&str_buf_3[0], bufinfo_3.buf, bufinfo_3.len);
    memcpy(&str_buf_3[bufinfo_3.len], "\0", 1);

    MP_THREAD_GIL_EXIT();
    nvs_write_string(str_buf_1, str_buf_2, str_buf_3);
    free(str_buf_1);
    free(str_buf_2);
    free(str_buf_3);

    MP_THREAD_GIL_ENTER();

    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(nvs_write_string_obj, 3, 3, mpy_nvs_write_string);

STATIC mp_obj_t mpy_nvs_read_string(mp_obj_t arg1, mp_obj_t arg2)
{
    char str_buf[NVS_STRING_LENGTH_MAX];

    mp_buffer_info_t bufinfo_1;
    mp_get_buffer_raise(arg1, &bufinfo_1, MP_BUFFER_READ);
    char *str_buf_1;
    str_buf_1 = (char *)malloc(bufinfo_1.len + 1);
    memcpy(&str_buf_1[0], bufinfo_1.buf, bufinfo_1.len);
    memcpy(&str_buf_1[bufinfo_1.len], "\0", 1);

    mp_buffer_info_t bufinfo_2;
    mp_get_buffer_raise(arg2, &bufinfo_2, MP_BUFFER_READ);
    char *str_buf_2;
    str_buf_2 = (char *)malloc(bufinfo_2.len + 1);
    memcpy(&str_buf_2[0], bufinfo_2.buf, bufinfo_2.len);
    memcpy(&str_buf_2[bufinfo_2.len], "\0", 1);

    MP_THREAD_GIL_EXIT();
    int result = nvs_read_string(str_buf_1, str_buf_2, str_buf);
    free(str_buf_1);
    free(str_buf_2);
    MP_THREAD_GIL_ENTER();
    if(result != 0)
    {
        return mp_const_none;
    }
    return mp_obj_new_str(str_buf, strlen(str_buf));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(nvs_read_string_obj, mpy_nvs_read_string);

STATIC mp_obj_t mpy_get_mac(void)
{
    uint8_t chipid[6];
    esp_read_mac(chipid, ESP_MAC_BT);    
    return mp_obj_new_bytes(chipid, 6);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_mac_obj, mpy_get_mac);

STATIC mp_obj_t mpy_get_firmware_version(void)
{
    return mp_obj_new_str(SOFT_VERSION, strlen(SOFT_VERSION));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(nvs_get_firmware_version_obj, mpy_get_firmware_version);

extern bool first_start_flag;

STATIC mp_obj_t mpy_is_first_start(void)
{
    return mp_obj_new_int(first_start_flag);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(is_first_start_obj, mpy_is_first_start);

STATIC const mp_map_elem_t matatalab_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_OFF),                          MP_OBJ_NEW_SMALL_INT(LED_BLUE_OFF) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON),                           MP_OBJ_NEW_SMALL_INT(LED_BLUE_ON) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SINGLE_FLASH),                 MP_OBJ_NEW_SMALL_INT(LED_BLUE_SINGLE_FLASH) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_DOUBLE_FLASH),                 MP_OBJ_NEW_SMALL_INT(LED_BLUE_DOUBLE_FLASH) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SINGLE_FAST_FLASH),            MP_OBJ_NEW_SMALL_INT(LED_BLUE_SINGLE_FAST_FLASH) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_indicator_led),                (mp_obj_t)(&indicator_led_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_usb_detected),              (mp_obj_t)(&is_usb_detected_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_battery_voltage),          (mp_obj_t)(&get_battery_voltage_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_battery_capacity),         (mp_obj_t)(&get_battery_capacity_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_power_off),                    (mp_obj_t)(&power_off_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_power_on),                     (mp_obj_t)(&power_on_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_power_state),              (mp_obj_t)(&get_power_state_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_nvs_write_string),             (mp_obj_t)&nvs_write_string_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_nvs_read_string),              (mp_obj_t)&nvs_read_string_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_mac),                      (mp_obj_t)&get_mac_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_version),                  (mp_obj_t)&nvs_get_firmware_version_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_first_start),               (mp_obj_t)&is_first_start_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_event),                        (mp_obj_t)&mt_mpy_event_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_ENABLE),                 MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_stop_script),                (mp_obj_t)&mt_mpy_stop_thread_type},
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_STOP_PYTHON_THREAD_ENABLE),  MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
#if MODULE_BUTTON_ENABLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_button),                       (mp_obj_t)&mpy_button_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_ENABLE),                MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_ENABLE),                MP_OBJ_NEW_SMALL_INT(MODULE_DISABLE_VALUE) },    
#endif /* MODULE_BUTTON_ENABLE */

#if MODULE_LEDS_ENABLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_leds),                         (mp_obj_t)&mpy_leds_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_LEDS_ENABLE),                  MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_LEDS_ENABLE),                  MP_OBJ_NEW_SMALL_INT(MODULE_DISABLE_VALUE) },
#endif /*MODULE_LEDS_ENABLE */

#if MODULE_LED_MATRIX_ENABLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_led_matrix),                   (mp_obj_t)&mpy_led_matrix_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_LED_MATRIX_ENABLE),            MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_LED_MATRIX_ENABLE),            MP_OBJ_NEW_SMALL_INT(MODULE_DISABLE_VALUE) },
#endif /*MODULE_LED_MATRIX_ENABLE */

#if MODULE_MOTION_ENABLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_motion),                       (mp_obj_t)&mpy_motion_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOTION_ENABLE),                MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOTION_ENABLE),                MP_OBJ_NEW_SMALL_INT(MODULE_DISABLE_VALUE) },
#endif /*MODULE_MOTION_ENABLE */

#if MODULE_SENSOR_ENABLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_sensor),                       (mp_obj_t)&mpy_sensor_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SENSOR_ENABLE),                MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_SENSOR_ENABLE),                MP_OBJ_NEW_SMALL_INT(MODULE_DISABLE_VALUE) },
#endif /*MODULE_SENSOR_ENABLE */

    { MP_OBJ_NEW_QSTR(MP_QSTR_ble),                          (mp_obj_t)(&mt_mpy_ble_type)},
};
STATIC MP_DEFINE_CONST_DICT(matatalab_module_globals, matatalab_module_globals_table);

const mp_obj_module_t matatalab_module = 
{
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&matatalab_module_globals,
};

