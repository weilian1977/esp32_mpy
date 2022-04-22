#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "usb_detect.h"
#include "system_management.h"
#include "indicator_led.h"

static const char *TAG = "INDICATOR LED";

static indicator_led_state_t indicator_led_state = LED_BLUE_SINGLE_FLASH;
static uint16_t single_flash_periodic_time = LED_SINGLE_FLASH_PERIODIC_TIME;

static void indicator_led_on(bool state)
{
    if(is_usb_detected())
    {
        gpio_set_level(INDICATOR_LED_PIN, 0);
    }
    else if(state)
    {
        gpio_set_level(INDICATOR_LED_PIN, 1);
    }
    else
    {
        gpio_set_level(INDICATOR_LED_PIN, 0);
    }
}

static void indicator_led_single_blue_flash(void)
{
    static bool led_status = false;
    static uint16_t single_flash_periodic_count = 0;
    single_flash_periodic_count++;
    if(single_flash_periodic_count > (single_flash_periodic_time / SYSTEM_POLLING_TIME))
    {
        led_status = !led_status;
        single_flash_periodic_count = 0;
    }
    if(led_status == true)
    {
        indicator_led_on(true);
    }
    else
    {
        indicator_led_on(false);
    }
}

static void indicator_led_double_blue_flash(void)
{
    static uint16_t double_flash_periodic_count = 0;
    double_flash_periodic_count++;
    if((double_flash_periodic_count / (LED_DOUBLE_FLASH_PERIODIC_TIME / SYSTEM_POLLING_TIME)) == 0)
    {
        indicator_led_on(true);
    }
    if((double_flash_periodic_count / (LED_DOUBLE_FLASH_PERIODIC_TIME / SYSTEM_POLLING_TIME)) == 1)
    {
        indicator_led_on(false);
    }
    if((double_flash_periodic_count / (LED_DOUBLE_FLASH_PERIODIC_TIME / SYSTEM_POLLING_TIME)) == 2)
    {
        indicator_led_on(true);
    }
    if((double_flash_periodic_count / (LED_DOUBLE_FLASH_PERIODIC_TIME / SYSTEM_POLLING_TIME)) == 3)
    {
        indicator_led_on(false);
    }
    if(double_flash_periodic_count > (LED_DOUBLE_FLASH_GAP_TIME / SYSTEM_POLLING_TIME))
    {
        double_flash_periodic_count = 0;
    }
}

void indicator_led_init(void)
{
    gpio_set_direction(INDICATOR_LED_PIN, GPIO_MODE_OUTPUT);
    indicator_led_set(2);
}

void indicator_led_set(indicator_led_state_t led_state)
{
    indicator_led_state = led_state;
    ESP_LOGD(TAG, "indicator_led: %d", indicator_led_state);
    if(LED_BLUE_SINGLE_FLASH == led_state)
    {
        single_flash_periodic_time = LED_SINGLE_FLASH_PERIODIC_TIME;
    }
    else if(LED_BLUE_SINGLE_FAST_FLASH == led_state)
    {
       
        single_flash_periodic_time = LED_SINGLE_FAST_FLASH_PERIODIC_TIME;
    }
}

void indicator_led_update(void)
{
    switch(indicator_led_state)
    {
        case LED_BLUE_OFF:
            indicator_led_on(false);
            break;

        case LED_BLUE_ON:
            indicator_led_on(true);
            break;

        case LED_BLUE_SINGLE_FLASH:
        case LED_BLUE_SINGLE_FAST_FLASH:
            indicator_led_single_blue_flash();
            break;

        case LED_BLUE_DOUBLE_FLASH:
            indicator_led_double_blue_flash();
            break;

        default:
            indicator_led_on(false);
    }
}
