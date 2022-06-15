#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "drv_i2c.h"
#include "drv_ltr381.h"
#include "drv_bh1745.h"

static const char *TAG = "DRV BH1745";
static bool bh1745_initialized = false;

static bool check_chip_id(void)
{
    esp_err_t ret = ESP_OK;
    uint8_t val = 0;
    bool id_sta = false;
    ret = i2c_master_read_reg(I2C0_MASTER_NUM, COLORSENSOR_DEFAULT_ADDRESS, MANUFACTURER_ID, &val);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "read MANUFACTURER_ID error!");
    }

    if(CHIP_ID == val){
        id_sta = true;
    }
    ESP_LOGE(TAG, "check_chip_id 0x%x!", val);
    return id_sta;
}

void bh1745_sensor_update(void)
{
    static uint16_t time_count = 0;
    if(time_count > 4)
    {
        uint8_t ColorData[8] = {0};
        i2c_master_read_mem(I2C0_MASTER_NUM, COLORSENSOR_DEFAULT_ADDRESS, RED_DATA_LSBs, ColorData, sizeof(ColorData));
        uint16_t Redvalue   = (uint16_t)ColorData[1] << 8 | ColorData[0];
        uint16_t Greenvalue = (uint16_t)ColorData[3] << 8 | ColorData[2];
        uint16_t Bluevalue  = (uint16_t)ColorData[5] << 8 | ColorData[4];
        uint16_t Colorvalue = (uint16_t)ColorData[7] << 8 | ColorData[6];
        printf("red:%d, green:%d, blue:%d, color:%d\r\n", Redvalue, Greenvalue, Bluevalue, Colorvalue);
        time_count = 0;
    }
    time_count++;
}

esp_err_t bh1745_init(void)
{
    esp_err_t ret = ESP_OK;
    if(bh1745_initialized == true)
    {
        ESP_LOGD(TAG, "bh1745 driver has been Initialized");
        return ret;
    }

    if(!is_i2c0_initialized())
    {
        i2c_master_init(I2C_NUM_0);
    }

    if(check_chip_id() == true)
    {
        bh1745_initialized = true;
    }

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, COLORSENSOR_DEFAULT_ADDRESS, SYSTEM_CONTROL, SW_RESET | INT_RESET);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write SYSTEM_CONTROL error!");
        return ret;
    }

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, COLORSENSOR_DEFAULT_ADDRESS, MODE_CONTROL1, MEASURE_160MS);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write MODE_CONTROL1 error!");
        return ret;
    }

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, COLORSENSOR_DEFAULT_ADDRESS, MODE_CONTROL2, 0x10); //active and set rgb measure gain
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write MODE_CONTROL2 error!");
        return ret;
    }

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, COLORSENSOR_DEFAULT_ADDRESS, INTERRUPT_REG, 0x00);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write INTERRUPT error!");
        return ret;
    }

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, COLORSENSOR_DEFAULT_ADDRESS, PERSISTENCE, 0x01);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write PERSISTENCE error!");
        return ret;
    }

    gpio_reset_pin(WHITE_LED_PIN);
    gpio_set_direction(WHITE_LED_PIN, GPIO_MODE_OUTPUT);
    white_led_on(true);
    return ret;
}

bool is_bh1745_initialized(void)
{
    return bh1745_initialized;
}