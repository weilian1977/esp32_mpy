#include <stdio.h>
#include <math.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "battery_check.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "battery check";

static esp_adc_cal_characteristics_t adc_chars;
static float battery_voltage = 0;
static float hardware_version_voltage = 0;
static uint8_t battery_capacity = 0;
static const battery_capacity_table_t battery_capacity_table[BATTERY_CAPACITY_TABLE_LEN] =
{
    { 4.2f, 100 },
    { 4.07f, 90 },
    { 3.97f, 80 },
    { 3.87f, 70 },
    { 3.80f, 60 },
    { 3.75f, 50 },
    { 3.70f, 40 },
    { 3.66f, 30 },
    { 3.63f, 20 },
    { 3.60f, 13 },
    { 3.57f, 8  },
    { 3.55f, 5  },
    { 3.30f, 0  }
};

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if(val_type == ESP_ADC_CAL_VAL_EFUSE_TP) 
    {
        ESP_LOGI(TAG, "Characterized using Two Point Value");
    }
    else if(val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        ESP_LOGI(TAG, "Characterized using eFuse Vref");
    } 
    else
    {
        ESP_LOGI(TAG, "Characterized using Default Vref");
    }
}

static uint32_t battery_check_get_value_and_filter(void)
{
    uint16_t val_max = 0;
    uint16_t val_min = 4096;
    int32_t adc_data;
    uint32_t value;
    float sum = 0.0f;
    for(uint8_t i = 0; i < 10; i++)
    {
        adc_data = adc1_get_raw(BATTERY_CHECK_CHANNEL);
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

static uint32_t hardware_version_get_value_and_filter(void)
{
    uint16_t val_max = 0;
    uint16_t val_min = 4096;
    int32_t adc_data;
    uint32_t value;
    float sum = 0.0f;
    for(uint8_t i = 0; i < 10; i++)
    {
        adc_data = adc1_get_raw(HARDWARE_VERSION_CHECK_CHANNEL);
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

void battery_check_init(void)
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(BATTERY_CHECK_CHANNEL, ADC_ATTEN_11db);
    //Characterize ADC
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);
    print_char_val_type(val_type);
    battery_voltage_update();
}

void hardware_version_check_init(void)
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(HARDWARE_VERSION_CHECK_CHANNEL, ADC_ATTEN_11db);
    //Characterize ADC
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);
    print_char_val_type(val_type);
    hardware_version_voltage_update();
}

void battery_voltage_update(void)
{
    uint32_t battery_current_ad_value = battery_check_get_value_and_filter();
    battery_voltage = (float)(esp_adc_cal_raw_to_voltage(battery_current_ad_value, &adc_chars) / BATTERY_VOLTAGE_DAMPING);
}

void hardware_version_voltage_update(void)
{
    uint32_t hardware_version_ad_value = hardware_version_get_value_and_filter();
    hardware_version_voltage = (float)(esp_adc_cal_raw_to_voltage(hardware_version_ad_value, &adc_chars) / 1000.0f);
}

void battery_capacity_update(void)
{
    for(uint8_t i = 0; i < BATTERY_CAPACITY_TABLE_LEN; i++)
    {
        if(battery_voltage > battery_capacity_table[i].battery_voltage)
        {
            if(i == 0)
            {
                battery_capacity = battery_capacity_table[i].battery_capacity;
                break;
            }
            else
            {
                battery_capacity = battery_capacity_table[i].battery_capacity + \
                       (uint8_t)round((battery_voltage - battery_capacity_table[i].battery_voltage) * \
                                      (battery_capacity_table[i - 1].battery_capacity - battery_capacity_table[i].battery_capacity) / \
                                      (battery_capacity_table[i - 1].battery_voltage - battery_capacity_table[i].battery_voltage));
                break;
            }
        }
    }
    if(battery_voltage < battery_capacity_table[BATTERY_CAPACITY_TABLE_LEN - 1].battery_voltage)
    {
        battery_capacity = 0;
    }
}

float get_battery_voltage(void)
{
    return battery_voltage;
}

float get_hardware_version_voltage(void)
{
    return hardware_version_voltage;
}

uint8_t get_battery_capacity(void)
{
    return battery_capacity;
}
