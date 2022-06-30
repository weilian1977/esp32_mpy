#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "usb_detect.h"
#include "battery_check.h"
#include "indicator_led.h"
#include "power_management.h"
#include "system_management.h"
#include "firmware_version.h"
#include "drv_light_sensor.h"
#include "drv_infrared_tube.h"
#include "drv_aw20144.h"
#include "adc_check.h"
#include "drv_infrared_transceiver.h"

extern void mp_hal_delay_ms(uint32_t ms);

// static const char *TAG = "USER SYSTEM";

void system_status_update(void)
{
    indicator_led_update();
    adc_check_update();
    light_sensor_voltage_update();
    infrared_tube_sensor_voltage_update();
    ir_code_update();
    usb_status_update();
}

void system_management_task(void *pvParameter)
{
    power_pin_init();
    power_on();
    adc_check_init();
    if(is_aw20144_initialized() == false)
    {
        aw20144_init();
    }
    battery_check_init();
    light_sensor_init();
    infrared_tube_sensor_init();
    show_firmware_version_t();
    indicator_led_init();
    while(true)
    {       
        system_status_update();
        power_management();
        vTaskDelay(SYSTEM_POLLING_TIME / portTICK_PERIOD_MS);
    }
}

