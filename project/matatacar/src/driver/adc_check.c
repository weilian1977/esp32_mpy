#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc_cal.h"
#include "adc_check.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "ADC_CHECK";
static esp_adc_cal_characteristics_t adc_chars;
static float play_key_and_usb_detect_voltage = 0;
static float ab_key_detect_voltage = 0;
/********************************************************************
 DEFINE PRIVATE FUNCTIONS
 ********************************************************************/
static uint32_t play_key_and_usb_detect_get_value_and_filter(void)
{
    uint16_t val_max = 0;
    uint16_t val_min = 4096;
    int32_t adc_data;
    uint32_t value;
    float sum = 0.0f;
    for(uint8_t i = 0; i < 10; i++)
    {
        adc_data = adc1_get_raw(PLAY_KEY_AND_USB_DETECT_CHANNEL);
        sum += adc_data;
        if(adc_data > val_max)
        {
            val_max = adc_data;
        }
        if(adc_data < val_min)
        { 
            val_min = adc_data;
        }
    }
    sum = sum - val_max - val_min;
    value = (uint32_t)round(sum / 8.0f);
    return value;
}

static uint32_t ab_key_detect_get_value_and_filter(void)
{
    uint16_t val_max = 0;
    uint16_t val_min = 4096;
    int32_t adc_data;
    uint32_t value;
    float sum = 0.0f;
    for(uint8_t i = 0; i < 10; i++)
    {
        adc_data = adc1_get_raw(AB_KEY_DETECT_CHANNEL);
        sum += adc_data;
        if(adc_data > val_max)
        {
            val_max = adc_data;
        }
        if(adc_data < val_min)
        { 
            val_min = adc_data;
        }
    }
    sum = sum - val_max - val_min;
    value = (uint32_t)round(sum / 8.0f);
    return value;
}

void adc_check_init(void)
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(PLAY_KEY_AND_USB_DETECT_CHANNEL, ADC_ATTEN_11db);
    adc1_config_channel_atten(AB_KEY_DETECT_CHANNEL, ADC_ATTEN_11db);
    //Characterize ADC
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);
    (void)val_type;
    adc_check_update();
}

void adc_check_update(void)
{
    uint32_t play_key_and_usb_detect_ad_value = play_key_and_usb_detect_get_value_and_filter();
    play_key_and_usb_detect_voltage = (float)(esp_adc_cal_raw_to_voltage(play_key_and_usb_detect_ad_value, &adc_chars) / 1000.0f);
    uint32_t ab_key_detect_ad_value = ab_key_detect_get_value_and_filter();
    ab_key_detect_voltage = (float)(esp_adc_cal_raw_to_voltage(ab_key_detect_ad_value, &adc_chars) / 1000.0f);
}

float get_play_key_and_usb_detect_voltage(void)
{
    return play_key_and_usb_detect_voltage;
}

float get_ab_key_detect_voltage(void)
{
    return ab_key_detect_voltage;
}