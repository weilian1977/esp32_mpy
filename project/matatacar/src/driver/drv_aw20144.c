#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "drv_i2c.h"
#include "drv_aw20144.h"

#define REG_WB_DEFAULT_VALUE  0x3F

static const char *TAG = "DRV AW20144";
static bool aw20144_initialized = false;

static uint8_t show_table_1[16] = 
{
    0x00, 0x00, 0x00, 0x00, 0x06, 0x60, 0x0f, 0xf0, 0x0f, 0xf0,
    0x0f, 0xf0, 0x0f, 0xf0, 0x06, 0x60,
};

static uint8_t show_table_2[16] = 
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x60, 0x0f, 0xf0,
    0x0f, 0xf0, 0x06, 0x60, 0x00, 0x00,
};

static uint8_t show_table_3[16] = 
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0,
    0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00,
};

static esp_err_t aw20144_soft_rst(void)
{
    esp_err_t ret = ESP_OK;
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE0);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page0 for page addr error!");
        return ret;
    }
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, REG_RSTN, 0xae);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write REG_RSTN reg error!");
        return ret;
    }
    ESP_LOGE(TAG, "soft rst ok!");
    vTaskDelay(10 / portTICK_PERIOD_MS);
    return ret;
}

static esp_err_t aw20144_chip_swen(void)
{
    esp_err_t ret = ESP_OK;
    uint8_t val = 0;
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE0);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page0 for page addr error!");
        return ret;
    }
    i2c_master_read_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, REG_GCR, &val);
    val &= BIT_CHIPEN_DIS;
    val |= BIT_CHIPEN_EN;
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, REG_GCR, val);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write REG_GCR reg error!");
        return ret;
    }
    ESP_LOGE(TAG, "aw20144_chip_swen %d!", val);
    return ret;
}

static bool check_chip_id(void)
{
    uint8_t val = 0;
    bool id_sta = false;
    i2c_master_read_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, REG_RSTN, &val);
    if(AW20144_CHIPID == val){
        id_sta = true;
    }
    ESP_LOGE(TAG, "check_chip_id %d!", val);
    return id_sta;
}

static esp_err_t aw20144_set_global_current(uint8_t current)
{
    esp_err_t ret = ESP_OK;
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE0);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page0 for page addr error!");
        return ret;
    }
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, REG_GCCR, current);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write REG_GCCR reg error!");
        return ret;
    }
    return ret;
}

static esp_err_t aw20144_set_constant_current_by_idx(uint8_t idx, uint8_t constant_current)
{
    esp_err_t ret = ESP_OK;
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, idx, constant_current);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write idx reg error!");
        return ret;
    }
    return ret;
}

static esp_err_t aw20144_set_pwm_by_idx(uint8_t idx, unsigned char pwm)
{
    esp_err_t ret = ESP_OK;
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, idx, pwm);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write idx reg error!");
        return ret;
    }
    return ret;
}

void aw20144_fast_clear_display(void)
{
    int i = 0;
    esp_err_t ret = ESP_OK;

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE1);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page1 for page addr error!");
    }

    for(i = 0;i < AW20XXX_REG_NUM_PAG1; i++){
        aw20144_set_pwm_by_idx(i, 0x00);
    }

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE2);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page2 for page addr error!");
    }

    for(i = 0;i < AW20XXX_REG_NUM_PAG2; i++){
        aw20144_set_constant_current_by_idx(i, 0x00);
    }
}

void aw20144_set_rgb_leds_brightness(uint8_t red_value, uint8_t green_value, uint8_t blue_value)
{
    esp_err_t ret = ESP_OK;

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE2);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page2 for page addr error!");
    }

    /* R colour brightness set*/
    aw20144_set_constant_current_by_idx(LEFT_COLOR_LEDS_RED_ID, red_value);
    /* G colour brightness set*/
    aw20144_set_constant_current_by_idx(LEFT_COLOR_LEDS_GREEN_ID, green_value);
    /* B colour brightness set*/
    aw20144_set_constant_current_by_idx(LEFT_COLOR_LEDS_BLUE_ID, blue_value);


    /* R colour brightness set*/
    aw20144_set_constant_current_by_idx(RIGHT_COLOR_LEDS_RED_ID, red_value);
    /* G colour brightness set*/
    aw20144_set_constant_current_by_idx(RIGHT_COLOR_LEDS_GREEN_ID, green_value);
    /* B colour brightness set*/
    aw20144_set_constant_current_by_idx(RIGHT_COLOR_LEDS_BLUE_ID, blue_value);
}

void aw20144_set_monochrome_leds_brightness(uint8_t brightness)
{
    uint16_t i = 0;
    esp_err_t ret = ESP_OK;

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE2);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page2 for page addr error!");
    }
    for (i = 0; i < AW20XXX_REG_NUM_PAG2; i++) {
        /* brightness set*/
        aw20144_set_constant_current_by_idx(i, brightness);
    }
}

static uint8_t table_map(uint8_t table_bit)
{
    uint8_t led_index;
    led_index = (18 * (table_bit / 16)) + (table_bit % 16);
    return led_index;
}

void aw20144_show_table_1(void)
{
    int i = 0;
    int j = 0;
    esp_err_t ret = ESP_OK;

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE1);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page1 for page addr error!");
    }

    for (i = 0; i < 16; i++)
    {
        uint8_t bit8_data = show_table_1[i];
        uint8_t bit_value = 0x00;
        for(j = 0; j < 8; j++)
        {
            bit_value = bit8_data & 0x01;
            if(bit_value == 0x01)
            {
                aw20144_set_pwm_by_idx(table_map(i * 8 + j), AW20XXX_MAX_PWM);
            }
            else
            {
                aw20144_set_pwm_by_idx(table_map(i * 8 + j), 0x00);
            }
            bit8_data = bit8_data >> 1;
        }
    }
}

void aw20144_show_table_2(void)
{
    int i = 0;
    int j = 0;
    esp_err_t ret = ESP_OK;

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE1);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page1 for page addr error!");
    }

    for (i = 0; i < 16; i++)
    {
        uint8_t bit8_data = show_table_2[i];
        uint8_t bit_value = 0x00;
        for(j = 0; j < 8; j++)
        {
            bit_value = bit8_data & 0x01;
            if(bit_value == 0x01)
            {
                aw20144_set_pwm_by_idx(table_map(i * 8 + j), AW20XXX_MAX_PWM);
            }
            else
            {
                aw20144_set_pwm_by_idx(table_map(i * 8 + j), 0x00);
            }
            bit8_data = bit8_data >> 1;
        }
    }
}

void aw20144_show_table_3(void)
{
    int i = 0;
    int j = 0;
    esp_err_t ret = ESP_OK;

    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE1);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page1 for page addr error!");
    }

    for (i = 0; i < 16; i++)
    {
        uint8_t bit8_data = show_table_3[i];
        uint8_t bit_value = 0x00;
        for(j = 0; j < 8; j++)
        {
            bit_value = bit8_data & 0x01;
            if(bit_value == 0x01)
            {
                aw20144_set_pwm_by_idx(table_map(i * 8 + j), AW20XXX_MAX_PWM);
            }
            else
            {
                aw20144_set_pwm_by_idx(table_map(i * 8 + j), 0x00);
            }
            bit8_data = bit8_data >> 1;
        }
    }
}

void set_left_rgb_led(uint8_t red, uint8_t green, uint8_t blue)
{
    esp_err_t ret = ESP_OK;
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE1);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page1 for page addr error!");
    }
    aw20144_set_pwm_by_idx(LEFT_COLOR_LEDS_RED_ID, red);
    aw20144_set_pwm_by_idx(LEFT_COLOR_LEDS_GREEN_ID, green);
    aw20144_set_pwm_by_idx(LEFT_COLOR_LEDS_BLUE_ID, blue);
}

void set_right_rgb_led(uint8_t red, uint8_t green, uint8_t blue)
{
    esp_err_t ret = ESP_OK;
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, AW20XXX_PAGE_ADDR, AW20XXX_CMD_PAGE1);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set page1 for page addr error!");
    }
    aw20144_set_pwm_by_idx(RIGHT_COLOR_LEDS_RED_ID, red);
    aw20144_set_pwm_by_idx(RIGHT_COLOR_LEDS_GREEN_ID, green);
    aw20144_set_pwm_by_idx(RIGHT_COLOR_LEDS_BLUE_ID, blue);
}

esp_err_t aw20144_init(void)
{
    esp_err_t ret = ESP_OK;
    if(aw20144_initialized == true)
    {
        ESP_LOGD(TAG, "aw20144 driver has been Initialized");
        return ret;
    }
    if(!is_i2c0_initialized())
    {
        i2c_master_init(I2C_NUM_0);
    }

    aw20144_soft_rst();
    aw20144_chip_swen();
    if(check_chip_id() == true)
    {
        aw20144_initialized = true;
    }
    aw20144_set_global_current(0xff);

    /* set constant current for monochrome leds broghtness */
    aw20144_set_monochrome_leds_brightness(0x20);
    aw20144_set_rgb_leds_brightness(AW20XXX_RSL_SET, AW20XXX_GSL_SET, AW20XXX_BSL_SET);

    /* low power mode */
    ret = i2c_master_write_reg(I2C0_MASTER_NUM, AW20144_I2C_ADDRESS, REG_MIXCR, 0x04);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "write REG_MIXCR reg error!");
    }
    return ret;
}

bool is_aw20144_initialized(void)
{
    return aw20144_initialized;
}