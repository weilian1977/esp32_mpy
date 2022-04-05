#include <stdio.h>
#include <math.h>
#include "driver/gpio.h"
#include "usb_detect.h"
#include "esp_err.h"
#include "esp_log.h"

//static const char *TAG = "usb detect";
static bool usb_detected = false;

static bool usb_detected_pin(void)
{
    return !gpio_get_level(USB_DETECT_PIN);
}

void usb_pin_init(void)
{
    gpio_set_direction(USB_DETECT_PIN, GPIO_MODE_INPUT);
}

void usb_status_update(void)
{
    usb_detected = usb_detected_pin();
}

bool is_usb_detected(void)
{
    return usb_detected;
}