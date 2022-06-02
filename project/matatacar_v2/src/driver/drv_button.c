#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "drv_button.h"
#include "adc_check.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "DRV_BUTTON";

static bool get_button_state(uint8_t button_id)
{
    bool button_state = false;
    float adc_voltage = 0;
    switch(button_id)
    {
        case BUTTON_PLAY:
            adc_voltage = get_play_key_and_usb_detect_voltage();
            if(((adc_voltage >  (PLAY_KEY_PRESSED - ADC_DETECT_OFFSET)) && 
               (adc_voltage <  (PLAY_KEY_PRESSED + ADC_DETECT_OFFSET))) ||
               ((adc_voltage >  (USB_INSERT_AND_PLAY_KEY_PRESSED - ADC_DETECT_OFFSET)) &&
               (adc_voltage <  (USB_INSERT_AND_PLAY_KEY_PRESSED + ADC_DETECT_OFFSET))))
            {
                button_state = true;
            }
            else
            {
            	button_state = false;
            }
            break;
        case BUTTON_A:
            adc_voltage = get_ab_key_detect_voltage();
            if(((adc_voltage >  (A_KEY_PRESSED - ADC_DETECT_OFFSET)) && 
               (adc_voltage <  (A_KEY_PRESSED + ADC_DETECT_OFFSET))) ||
               ((adc_voltage >  (A_KEY_PRESSED_AND_B_KEY_PRESSED - ADC_DETECT_OFFSET)) &&
               (adc_voltage <  (A_KEY_PRESSED_AND_B_KEY_PRESSED + ADC_DETECT_OFFSET))))
            {
                button_state = true;
            }
            else
            {
            	button_state = false;
            }
            break;
        case BUTTON_B:
            adc_voltage = get_ab_key_detect_voltage();
            if(((adc_voltage >  (B_KEY_PRESSED - ADC_DETECT_OFFSET)) &&
               (adc_voltage <  (B_KEY_PRESSED + ADC_DETECT_OFFSET))) ||
               ((adc_voltage >  (A_KEY_PRESSED_AND_B_KEY_PRESSED - ADC_DETECT_OFFSET)) &&
               (adc_voltage <  (A_KEY_PRESSED_AND_B_KEY_PRESSED + ADC_DETECT_OFFSET))))
            {
                button_state = true;
            }
            else
            {
            	button_state = false;
            }
            break;
        default:
            ESP_LOGE(TAG, "button does not exist!");
            return false;
    }	
    return button_state;
}

uint8_t get_button_value(void)
{
    bool a_button_value = get_button_state(BUTTON_A);
    bool b_button_value = get_button_state(BUTTON_B);
    bool play_button_value = get_button_state(BUTTON_PLAY);
    uint8_t button_value = 0;
    button_value = ((a_button_value << 2) | (b_button_value << 1) | play_button_value) & 0x07;
    return button_value;
}

bool is_key_pressed(uint8_t button_id)
{
    return get_button_state(button_id);
}

bool is_key_released(uint8_t button_id)
{
    return !get_button_state(button_id);
}