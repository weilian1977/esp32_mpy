#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_adc_cal.h"
#include "drv_infrared_tube.h"
#include "adc_check.h"

static const char *TAG = "INFRARED_TUBE_SENSOR";

static esp_adc_cal_characteristics_t adc_chars;
static float infrared_tube_sensor_voltage[INFRARED_TUBE_SENSOR_MAXNUM] = {0};

static uint32_t line_follower_1_get_value_and_filter(void)
{
    uint16_t val_max = 0;
    uint16_t val_min = 4096;
    int32_t adc_data;
    uint32_t value;
    // float sum = 0.0f;
    // for(uint8_t i = 0; i < 10; i++)
    // {
    //     adc_data = adc1_get_raw(INFRARED_TUBE_LINE_FOLLOWER_1);
    //     sum += adc_data;
    //     if(adc_data > val_max)
    //     {
    //         val_max = adc_data;
    //     }
    //     if(adc_data < val_min)
    //     { 
    //         val_min = adc_data;
    //     }
    // }
    // sum = sum - val_max - val_min;
    // value = (uint32_t)round(sum / 8.0f);
    return value;
}

static uint32_t line_follower_2_get_value_and_filter(void)
{
    uint16_t val_max = 0;
    uint16_t val_min = 4096;
    int32_t adc_data;
    uint32_t value;
    // float sum = 0.0f;
    // for(uint8_t i = 0; i < 10; i++)
    // {
    //     adc_data = adc1_get_raw(INFRARED_TUBE_LINE_FOLLOWER_2);
    //     sum += adc_data;
    //     if(adc_data > val_max)
    //     {
    //         val_max = adc_data;
    //     }
    //     if(adc_data < val_min)
    //     { 
    //         val_min = adc_data;
    //     }
    // }
    // sum = sum - val_max - val_min;
    // value = (uint32_t)round(sum / 8.0f);
    return value;
}

static uint32_t obstacle_avoidance_get_value_and_filter(void)
{
    uint16_t val_max = 0;
    uint16_t val_min = 4096;
    int32_t adc_data;
    uint32_t value;
    float sum = 0.0f;
    for(uint8_t i = 0; i < 10; i++)
    {
        adc_data = adc1_get_raw(INFRARED_TUBE_OBSTACLE_AVOIDANCE);
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

void infrared_tube_sensor_init(void)
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(INFRARED_TUBE_OBSTACLE_AVOIDANCE, ADC_ATTEN_11db);
    //Characterize ADC
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);

    // adc1_config_width(ADC_WIDTH_12Bit);
    // adc1_config_channel_atten(INFRARED_TUBE_LINE_FOLLOWER_1, ADC_ATTEN_11db);
    // //Characterize ADC
    // val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);

    // adc1_config_width(ADC_WIDTH_12Bit);
    // adc1_config_channel_atten(INFRARED_TUBE_LINE_FOLLOWER_2, ADC_ATTEN_11db);
    // //Characterize ADC
    // val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);

    (void)val_type;
    infrared_tube_sensor_voltage_update();
}

void infrared_tube_sensor_voltage_update(void)
{
    uint32_t infrared_tube_sensor_ad_value = obstacle_avoidance_get_value_and_filter(); 
    infrared_tube_sensor_voltage[0] = (float)(esp_adc_cal_raw_to_voltage(infrared_tube_sensor_ad_value, &adc_chars) / 1000.0f);
    // infrared_tube_sensor_ad_value = line_follower_1_get_value_and_filter();
    // infrared_tube_sensor_voltage[1] = (float)(esp_adc_cal_raw_to_voltage(infrared_tube_sensor_ad_value, &adc_chars) / 1000.0f);
    // infrared_tube_sensor_ad_value = line_follower_2_get_value_and_filter();
    // infrared_tube_sensor_voltage[2] = (float)(esp_adc_cal_raw_to_voltage(infrared_tube_sensor_ad_value, &adc_chars) / 1000.0f);
}

float get_infrared_tube_value(uint8_t ir_channel)
{
    if(ir_channel == 0)
    {
        return infrared_tube_sensor_voltage[0];
    }
    else if(ir_channel == 1)
    {
        return infrared_tube_sensor_voltage[1];
    }
    else if(ir_channel == 2)
    {
        return infrared_tube_sensor_voltage[2];
    }
    else
    {
        return 0;
    }
}
