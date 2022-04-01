#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "esp_log.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "mt_err.h"

#include "mt_module_config.h"
#include "mt_light_sensor.h"

#include "esp_log.h"

#if MODULE_LIGHT_SENSOR_ENABLE
/************************************************************/
#define TAG                           ("mt_light_sensor")
#define ADC1_EXAMPLE_CHAN0          ADC1_CHANNEL_8

//ADC Attenuation
#define ADC_EXAMPLE_ATTEN           ADC_ATTEN_DB_11
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP_FIT

static int light_value;
static int adc_raw;
static esp_adc_cal_characteristics_t adc1_chars;
/******************************************************************************
 DECLARE PUBLIC FUNCTIONS  
 ******************************************************************************/


/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
static bool adc_calibration_init(void);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
mt_err_t mt_light_sensor_init_t(void)
{
  //esp_err_t ret = ESP_OK;
  bool cali_enable = adc_calibration_init();

  // ADC1 config
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
  ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));
  return MT_OK;
}

mt_err_t mt_light_sensor_update_t(void)
{
  adc_raw = adc1_get_raw(ADC1_EXAMPLE_CHAN0);
  //ESP_LOGI(TAG, "raw  data: %d", adc_raw);
  light_value = esp_adc_cal_raw_to_voltage(adc_raw, &adc1_chars);
  //ESP_LOGI(TAG, "light_value: %d", light_value);
  return MT_OK;
}

mt_err_t mt_light_sensor_get_value_t(int *value)
{
  (*value) = light_value;
  return MT_OK;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
static bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_EXAMPLE_ATTEN, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}
#endif /* MODULE_LIGHT_SENSOR_ENABLE */