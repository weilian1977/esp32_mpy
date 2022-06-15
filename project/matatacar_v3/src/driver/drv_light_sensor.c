#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_adc_cal.h"
#include "drv_light_sensor.h"
#include "adc_check.h"

static const char *TAG = "LIGHT_SENSOR";

static esp_adc_cal_characteristics_t adc_chars;
static float light_sensor_voltage[LIGHT_SENSOR_MAXNUM] = {0};

static uint32_t light_sensor_1_get_value_and_filter(void)
{
    uint16_t val_max = 0;
    uint16_t val_min = 4096;
    int32_t adc_data;
    uint32_t value;
    float sum = 0.0f;
    for(uint8_t i = 0; i < 10; i++)
    {
        adc_data = adc1_get_raw(LIGHT_SENSOR_CHANNEL_1);
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

static uint32_t light_sensor_2_get_value_and_filter(void)
{
    uint16_t val_max = 0;
    uint16_t val_min = 4096;
    int32_t adc_data;
    uint32_t value;
    float sum = 0.0f;
    for(uint8_t i = 0; i < 10; i++)
    {
        adc_data = adc1_get_raw(LIGHT_SENSOR_CHANNEL_2);
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

void light_sensor_init(void)
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(LIGHT_SENSOR_CHANNEL_1, ADC_ATTEN_11db);
    adc1_config_channel_atten(LIGHT_SENSOR_CHANNEL_2, ADC_ATTEN_11db);
    //Characterize ADC
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);
    (void)val_type;
    light_sensor_voltage_update();
}

void light_sensor_voltage_update(void)
{
    uint32_t light_sensor_ad_value = light_sensor_1_get_value_and_filter();
    light_sensor_voltage[0] = (float)(esp_adc_cal_raw_to_voltage(light_sensor_ad_value, &adc_chars) / 1000.0f);
    light_sensor_ad_value = light_sensor_2_get_value_and_filter();
    light_sensor_voltage[1] = (float)(esp_adc_cal_raw_to_voltage(light_sensor_ad_value, &adc_chars) / 1000.0f);
}

float get_light_value(uint8_t light_channel)
{
    if(light_channel == 0)
    {
        return light_sensor_voltage[0];
    }
    else if(light_channel == 1)
    {
        return light_sensor_voltage[1];
    }
    else
    {
        return 0;
    }
}