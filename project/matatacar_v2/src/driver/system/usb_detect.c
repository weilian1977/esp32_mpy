#include <stdio.h>
#include <math.h>
#include "driver/gpio.h"
#include "usb_detect.h"
#include "adc_check.h"
#include "esp_err.h"
#include "esp_log.h"

//static const char *TAG = "usb detect";
static bool usb_detected = false;

void usb_status_update(void)
{
    float adc_voltage = get_play_key_and_usb_detect_voltage();
    if(((adc_voltage >  (USB_INSERT - ADC_DETECT_OFFSET)) &&
       (adc_voltage <  (USB_INSERT + ADC_DETECT_OFFSET))) ||
       ((adc_voltage >  (USB_INSERT_AND_PLAY_KEY_PRESSED - ADC_DETECT_OFFSET)) &&
       (adc_voltage <  (USB_INSERT_AND_PLAY_KEY_PRESSED + ADC_DETECT_OFFSET))))
    {
        usb_detected = true;
    }
    else
    {
        usb_detected = false;
    }
}

bool is_usb_detected(void)
{
    return usb_detected;
}