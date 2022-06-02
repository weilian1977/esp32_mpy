#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "drv_i2c.h"
#include "drv_ltr381.h"

static const char *TAG = "DRV LTR381";
static bool ltr381_initialized = false;
rgb_data_t rgb_value = {0};

static bool check_chip_id(void)
{
    esp_err_t ret = ESP_OK;
    uint8_t val = 0;
    bool id_sta = false;
    ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_PART_ID_REG, &val);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "read LTR381_PART_ID_REG error!");
    }

    if(LTR381_PART_ID_VAL == val){
        id_sta = true;
    }
    ESP_LOGE(TAG, "check_chip_id 0x%x!", val);
    return id_sta;
}

static esp_err_t ltr381_set_power_mode(uint8_t mode)
{
    esp_err_t ret = ESP_OK;
    uint8_t dev_mode = 0;
    uint8_t value = 0;
    ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_MAIN_CTRL_REG, &value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "read LTR381_MAIN_CTRL_REG error!");
        return ret;
    }

    switch (mode)
    {
        case DEV_POWER_OFF:
            dev_mode = LTR381_SET_BITSLICE(value, ALS_CS_ENABLE, LTR381_ALS_STANDBY_MODE);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR381_SET_BITSLICE(value, ALS_CS_ENABLE, LTR381_ALS_ACTIVE_MODE);
            break;
        default:
            return -1;
    }

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_MAIN_CTRL_REG, dev_mode);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_MAIN_CTRL_REG error!");
        return ret;
    }

    return ret;
}

static bool is_ltr381_ready(void)
{
    esp_err_t ret = ESP_OK;
    uint8_t value = 0;
    bool is_ready;
    ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_MAIN_STATUS_REG, &value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "read LTR381_MAIN_STATUS_REG error!");
        return false;
    }

    is_ready = (LTR381_GET_BITSLICE(value, ALS_CS_DATA_STATUS) == LTR381_CS_ALS_DATA_NEW) ? 1 : 0;

    return is_ready;
}

static esp_err_t ltr381_set_default_config(void)
{
    esp_err_t ret = ESP_OK;
    uint8_t value = 0;

    value = LTR381_SET_BITSLICE(value, ALS_CS_ENABLE, LTR381_ALS_STANDBY_MODE);
    value = LTR381_SET_BITSLICE(value, CS_MODE, LTR381_CS_MODE);
    value = LTR381_SET_BITSLICE(value, SW_RESET, LTR381_ALS_NO_RESET);
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_MAIN_CTRL_REG, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_MAIN_CTRL_REG error!");
        return ret;
    }

    value = 0;
    value = LTR381_SET_BITSLICE(value, ALS_CS_MEAS_RATE, LTR381_ALS_CS_MEAS_RATE_25MS);
    value = LTR381_SET_BITSLICE(value, ALS_CS_RESOLUTION, LTR381_ALS_CS_RESOLUTION_16BIT);
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_ALS_CS_MEAS_RATE_REG, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_ALS_CS_MEAS_RATE_REG error!");
        return ret;
    }

    value = 0;
    value = LTR381_SET_BITSLICE(value, ALS_CS_GAIN_RANGE, LTR381_ALS_CS_GAIN_RANGE_3);
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_ALS_CS_GAIN_REG, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_ALS_CS_GAIN_REG error!");
        return ret;
    }
    value = 0;
    value = LTR381_SET_BITSLICE(value, ALS_INT_PIN_EN, LTR381_ALS_INT_INACTIVE);
    value = LTR381_SET_BITSLICE(value, ALS_INT_SEL, LTR381_ALS_INT_GREEN_CHANNEL);
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_INT_CFG_REG, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_INT_CFG_REG error!");
        return ret;
    }

    value = 0;
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_INT_PST_REG, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_INT_PST_REG error!");
        return ret;
    }

    return ret;
}

static esp_err_t ltr381_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg_ch_red_data[3] = { 0 };
    uint8_t reg_ch_green_data[3] = { 0 };
    uint8_t reg_ch_blue_data[3] = { 0 };
    uint32_t ch_red_data = 0;
    uint32_t ch_green_data = 0;
    uint32_t ch_blue_data = 0;
    rgb_data_t * pdata = (rgb_data_t *) buf;

    if (buf == NULL){
        return -1;
    }

    size = sizeof(rgb_data_t);
    if (len < size){
        return -1;
    }

    if (!is_ltr381_ready()){
        return -1;
    }

    ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_CS_DATA_GREEN_0_REG, &reg_ch_green_data[0]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_CS_DATA_GREEN_0_REG error!");
        return ret;
    }

    ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_CS_DATA_GREEN_1_REG, &reg_ch_green_data[1]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_CS_DATA_GREEN_1_REG error!");
        return ret;
    }

    // ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_CS_DATA_GREEN_2_REG, &reg_ch_green_data[2]);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "write LTR381_CS_DATA_GREEN_2_REG error!");
    //     return ret;
    // }


    ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_CS_DATA_RED_0_REG, &reg_ch_red_data[0]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_CS_DATA_RED_0_REG error!");
        return ret;
    }

    ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_CS_DATA_RED_1_REG, &reg_ch_red_data[1]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_CS_DATA_RED_1_REG error!");
        return ret;
    }

    // ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_CS_DATA_RED_2_REG, &reg_ch_red_data[2]);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "write LTR381_CS_DATA_RED_2_REG error!");
    //     return ret;
    // }

    ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_CS_DATA_BLUE_0_REG, &reg_ch_blue_data[0]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_CS_DATA_BLUE_0_REG error!");
        return ret;
    }

    ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_CS_DATA_BLUE_1_REG, &reg_ch_blue_data[1]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write LTR381_CS_DATA_BLUE_1_REG error!");
        return ret;
    }

    // ret = i2c_master_read_reg(I2C0_MASTER_NUM, LTR381_SLAVE_ADDR, LTR381_CS_DATA_BLUE_2_REG, &reg_ch_blue_data[2]);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "write LTR381_CS_DATA_BLUE_2_REG error!");
    //     return ret;
    // }

    // ch_red_data = (((uint32_t) reg_ch_red_data[2] << 16) | ((uint32_t) reg_ch_red_data[1] << 8) | reg_ch_red_data[0]);
    // ch_green_data = (((uint32_t) reg_ch_green_data[2] << 16) | ((uint32_t) reg_ch_green_data[1] << 8) | reg_ch_green_data[0]);
    // ch_blue_data = (((uint32_t) reg_ch_blue_data[2] << 16) | ((uint32_t) reg_ch_blue_data[1] << 8) | reg_ch_blue_data[0]);

    ch_red_data = (((uint32_t) reg_ch_red_data[1] << 8) | reg_ch_red_data[0]);
    ch_green_data = (((uint32_t) reg_ch_green_data[1] << 8) | reg_ch_green_data[0]);
    ch_blue_data = (((uint32_t) reg_ch_blue_data[1] << 8) | reg_ch_blue_data[0]);
    pdata->data[0] = ch_red_data;
    pdata->data[1] = ch_green_data;
    pdata->data[2] = ch_blue_data;
    return ret;
}

void ltr381_sensor_update(void)
{
    rgb_data_t rgb_value_tmp = {0};
    if(ltr381_read(&rgb_value_tmp, 12) == 0)
    {
        rgb_value.data[0] = rgb_value_tmp.data[0];
        rgb_value.data[1] = rgb_value_tmp.data[1];
        rgb_value.data[2] = rgb_value_tmp.data[2];
    }
}

void white_led_on(bool state)
{
    if(state)
    {
        gpio_set_level(WHITE_LED_PIN, 1);
    }
    else
    {
        gpio_set_level(WHITE_LED_PIN, 0);
    }
}

esp_err_t ltr381_init(void)
{
    esp_err_t ret = ESP_OK;
    if(ltr381_initialized == true)
    {
        ESP_LOGD(TAG, "ltr381 driver has been Initialized");
        return ret;
    }
    if(!is_i2c0_initialized())
    {
        i2c_master_init(I2C_NUM_0);
    }

    if(check_chip_id() == true)
    {
        ltr381_initialized = true;
    }
    ltr381_set_default_config();
    ltr381_set_power_mode(DEV_POWER_ON);
    gpio_reset_pin(WHITE_LED_PIN);
    gpio_set_direction(WHITE_LED_PIN, GPIO_MODE_OUTPUT);
    white_led_on(true);
    return ret;
}

int32_t get_red_value(void)
{
    return rgb_value.data[0];
}

int32_t get_green_value(void)
{
    return rgb_value.data[1];
}

int32_t get_blue_value(void)
{
    return rgb_value.data[2];
}

bool is_ltr381_initialized(void)
{
    return ltr381_initialized;
}