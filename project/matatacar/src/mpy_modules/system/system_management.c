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
#include "drv_nvs.h"
#include "drv_ota.h"
#include "drv_motor.h"
#include "drv_pwm.h"
#include "drv_motor_control.h"
#include "drv_timer.h"
#include "firmware_version.h"
#include "drv_aw9106.h"

#include "drv_light_sleep.h"

extern void mp_hal_delay_ms(uint32_t ms);


static const char *TAG = "USER SYSTEM";

void system_status_update(void)
{
    usb_status_update();
    indicator_led_update();
}

void system_management_task(void *pvParameter)
{
    usb_pin_init();
    indicator_led_init();
    nvs_init();
    esp32_ota_config_init();  
    power_on();
    battery_check_init();
    hardware_version_check_init();
    power_pin_init();
    encode_init();
    mp_timer_init();
    aw9106_init();
    show_firmware_version_t();
    motion_data.motion_mode = CAR_STOP_RELEASE;
    motor_control_timer_init();
    while(true)
    {       
        system_status_update();
        power_management();
        vTaskDelay(SYSTEM_POLLING_TIME / portTICK_PERIOD_MS);
    }
}
