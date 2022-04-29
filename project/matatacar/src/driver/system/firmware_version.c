#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "esp_log.h"
#include "esp_adc_cal.h"
#include "battery_check.h"
#include "firmware_version.h"

/********************************************************************
 DEFINE MACROS
 ********************************************************************/
#define TAG   "FIRMWARE_VERSION"

/********************************************************************
 DEFINE PRIVATE DATAS
 ********************************************************************/
static esp_adc_cal_characteristics_t adc_chars;
static char s_firmware_version[FIRMWARE_VERSION_LENGTH_MAX] = SOFT_VERSION;
static float hardware_version_voltage = 0;

/********************************************************************
 DEFINE PUBLIC DATAS
 ********************************************************************/

/********************************************************************
 DEFINE PRIVATE FUNCTIONS
 ********************************************************************/
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

/********************************************************************
 DEFINE PUBLIC FUNCTIONS
 ********************************************************************/
const char *get_firmware_version_t(void)
{
  if(strlen(s_firmware_version) > FIRMWARE_VERSION_LENGTH_MAX)
  {
  	s_firmware_version[FIRMWARE_VERSION_LENGTH_MAX - 1] = '\0';
  }
  return s_firmware_version;
}

uint8_t get_hardware_version_t(void)
{
  uint8_t hardware_version = 0;
  if(hardware_version_voltage < 0.15f)
  {
    hardware_version = 0;
  }
  else if((hardware_version_voltage > 0.15f) && (hardware_version_voltage < (HARDWARE_VERSION_NUM1 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 1;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM2 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM2 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 2;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM3 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM3 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 3;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM4 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM4 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 4;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM5 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM5 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 5;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM6 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM6 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 6;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM7 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM7 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 7;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM8 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM8 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 8;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM9 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM9 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 9;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM10 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM10 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 10;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM11 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM11 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 11;
  }
  else if(hardware_version_voltage > 2.45f)
  {
    hardware_version = 12;
  }
  else
  {
    hardware_version = 255;
  }
  return hardware_version;
}

void hardware_version_check_init(void)
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(HARDWARE_VERSION_CHECK_CHANNEL, ADC_ATTEN_11db);
    //Characterize ADC
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);
    hardware_version_voltage_update();
}

void hardware_version_voltage_update(void)
{
    uint32_t hardware_version_ad_value = hardware_version_get_value_and_filter();
    hardware_version_voltage = (float)(esp_adc_cal_raw_to_voltage(hardware_version_ad_value, &adc_chars) / 1000.0f);
}

void show_firmware_version_t(void)
{
  printf("firmware version is:%s\n", s_firmware_version);
  printf("hardware version is:%d\n", get_hardware_version_t());
  printf("build time: %s, %s\n", __DATE__, __TIME__);
}

int firmware_version_append_t(char *str)
{
  int len = 0;
  len = strlen(str);

  if(len > (FIRMWARE_VERSION_LENGTH_MAX - 1))
  {
  	ESP_LOGE(TAG, "string is too long, %d, max is:%d", strlen(str), (FIRMWARE_VERSION_LENGTH_MAX - 1));
  	return -1;
  }
  if(strlen(s_firmware_version) + len > (FIRMWARE_VERSION_LENGTH_MAX - 1))
  {
  	ESP_LOGE(TAG, "version is too long:%d, max is:%d", strlen(s_firmware_version), (FIRMWARE_VERSION_LENGTH_MAX - 1));
  	return -1;
  }

  strcat(s_firmware_version, str);
  return 0;
}
