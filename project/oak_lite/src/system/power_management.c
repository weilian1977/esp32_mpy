#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "power_management.h"
#include "battery_check.h"
#include "esp_err.h"
#include "esp_log.h"

void power_management(void)
{
    battery_voltage_update();
    battery_capacity_update();
    //hardware_version_voltage_update();
}
