#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "indicator_led.h"
#include "power_management.h"
#include "battery_check.h"
#include "esp_err.h"
#include "esp_log.h"

// static const char *TAG = "power management";

static void power_keep(bool keep_power)
{
    gpio_set_level(SW_PIN, keep_power);
}

void power_on(void)
{
    power_keep(true);
}

void power_pin_init(void)
{
    gpio_pad_select_gpio(POWER_PIN);
    gpio_pad_select_gpio(SW_PIN);
    gpio_set_direction(POWER_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SW_PIN, GPIO_MODE_OUTPUT);
}

bool get_power_pin_state(void)
{
    return !(gpio_get_level(POWER_PIN));
}

void power_off(void)
{
    power_keep(false);
}

void power_management(void)
{
    battery_voltage_update();
    battery_capacity_update();
    hardware_version_voltage_update();
}
