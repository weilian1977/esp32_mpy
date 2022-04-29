
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2s.h"
#include "drv_i2s.h"
#include "soc/soc.h"

#include "mphalport.h"

static int i2s0_acquire_index=0;

i2s_config_t my_i2s_config = {
    .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX,
    .sample_rate = 16000,
    .bits_per_sample = 16,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    //when dma_buf_count = 3 and dma_buf_len = 300, then 3 * 4 * 300 * 2 Bytes internal RAM will be used. The multiplier 2 is for Rx buffer and Tx buffer together.
    .dma_buf_count = 3,                            /*!< amount of the dam buffer sectors*/
    .dma_buf_len = 300,                            /*!< dam buffer size of each sector (word, i.e. 4 Bytes) */
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,
    .use_apll = 1,
};
const i2s_pin_config_t my_i2s_pin = {
    .bck_io_num = IIS_SCLK,
    .ws_io_num = IIS_LCLK,
    .data_out_num = IIS_DSIN,
    .data_in_num = IIS_DOUT
};


esp_err_t i2s0_init(void)
{
    int  ret = i2s_driver_install(I2S_NUM_0, &my_i2s_config, 0, NULL);
    if (ret < 0) {
        return ESP_FAIL;
    }
    SET_PERI_REG_BITS(PIN_CTRL, CLK_OUT1, 0, CLK_OUT1_S);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, 1);             //CHANGE
    ret = i2s_set_pin(I2S_NUM_0, &my_i2s_pin);
    return ret;
}


esp_err_t i2s0_set_sample_rates(uint32_t rate)
{
    int ret=i2s_set_sample_rates(I2S_NUM_0, rate);
    return ret;
}

esp_err_t i2s0_dac_audio_play(short * data, int length)
{
    size_t bytes_write = 0;
    i2s_write(0, (const char*) data, length, &bytes_write, portMAX_DELAY);
    return ESP_OK;
}

esp_err_t i2s0_shdn_enable(uint8_t en)
{
    gpio_pad_select_gpio(IIS_SHDN);
    gpio_set_direction(IIS_SHDN, GPIO_MODE_OUTPUT);
    gpio_set_level(IIS_SHDN, en);
    return ESP_OK;
}

esp_err_t i2s0_shdn_control(uint8_t count)
{
    int t;
    gpio_pad_select_gpio(IIS_SHDN);
    gpio_set_direction(IIS_SHDN, GPIO_MODE_OUTPUT);
    while(count-->0)
    {
        for(t=0;t<0xf;t++);
        gpio_set_level(IIS_SHDN, 0);
        for(t=0;t<0xf;t++);
        gpio_set_level(IIS_SHDN, 1);
    }
    return ESP_OK;
}

int i2s0_acquire_play()
{
    i2s0_acquire_index++;
    return i2s0_acquire_index;
}

esp_err_t i2s0_output_play(int acquire_id,short * data, int length)
{
    if(acquire_id==i2s0_acquire_index)
    {
        return i2s0_dac_audio_play(data,length);
    }
    else 
        return AUDIO_BREAK;
}

esp_err_t i2s0_output_plays(short * data, int length)//length为char字节长度
{
    int i=0;
    int half_bag = length%512;
    int bat_count = length/512;
    i2s0_shdn_enable(1);
    int id = i2s0_acquire_play();
    for(i = 0; i < bat_count; i++)
    {
        int ret=i2s0_output_play(id,&data[i*512], 512);
        if(ret==AUDIO_BREAK)
        {
            return AUDIO_BREAK;
        }
    }
    if(half_bag>0)
    {
        int ret=i2s0_output_play(id,&data[i*512], half_bag);
        if(ret==AUDIO_BREAK)
        {
            return AUDIO_BREAK;
        }
    }
    i2s0_shdn_enable(0);
    return ESP_OK;
}