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
//#include "mt_mpy_stop_script.h"
#include "mt_mpy_button.h"
#include "mt_mpy_gyro.h"
#include "mt_mpy_tof.h"
#include "mt_mpy_light_sensor.h"
#include "mt_mpy_ble.h"

#define   TAG                         ("matatalab")

#define MODULE_ENABLE_VALUE   (1)
#define MODULE_DISABLE_VALUE  (0)

STATIC const mp_map_elem_t matatalab_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_event),                      (mp_obj_t)&mt_mpy_event_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_EVENT_ENABLE),               MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_stop_script),                (mp_obj_t)&mt_mpy_stop_thread_type},
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_STOP_PYTHON_THREAD_ENABLE),  MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
#if MODULE_BUTTON_ENABLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_button),                      (mp_obj_t)&mt_mpy_button_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_ENABLE),                MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) },
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_ENABLE),                MP_OBJ_NEW_SMALL_INT(MODULE_DISABLE_VALUE) },    
#endif /* MODULE_BUTTON_ENABLE */
    
#if MODULE_GYRO_ENABLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_gyro),                        (mp_obj_t)&mt_mpy_gyro_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_GYRO_ENABLE),                  MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) }, 
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_GYRO_ENABLE),                  MP_OBJ_NEW_SMALL_INT(MODULE_DISABLE_VALUE) }, 
#endif /* MODULE_GYRO_ENABLE */

#if MODULE_TOF_ENABLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_tof),                        (mp_obj_t)&mt_mpy_tof_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_TOF_ENABLE),                  MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) }, 
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_TOF_ENABLE),                  MP_OBJ_NEW_SMALL_INT(MODULE_DISABLE_VALUE) }, 
#endif /* MODULE_TOF_ENABLE */

#if MODULE_LIGHT_SENSOR_ENABLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_light_sensor),                (mp_obj_t)&mt_mpy_light_sensor_type},
    { MP_OBJ_NEW_QSTR(MP_QSTR_LIGHT_SENSOR_ENABLE),         MP_OBJ_NEW_SMALL_INT(MODULE_ENABLE_VALUE) }, 
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_LIGHT_SENSOR_ENABLE),         MP_OBJ_NEW_SMALL_INT(MODULE_DISABLE_VALUE) }, 
#endif /* MODULE_LIGHT_SENSOR_ENABLE */

    { MP_OBJ_NEW_QSTR(MP_QSTR_ble),                   (mp_obj_t)(&mt_mpy_ble_type)},
};
STATIC MP_DEFINE_CONST_DICT(matatalab_module_globals, matatalab_module_globals_table);

const mp_obj_module_t matatalab_module = 
{
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&matatalab_module_globals,
};

