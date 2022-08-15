
#ifndef __DRV_I2S_H__
#define __DRV_I2S_H__

#include "esp_err.h"

#define IIS_SCLK                    9
#define IIS_LCLK                    45
#define IIS_DSIN                    10
#define IIS_DOUT                    8

#define IIS_SHDN                    46

#define AUDIO_OK                     0
#define AUDIO_FAIL                   1
#define AUDIO_BREAK                  3


esp_err_t i2s0_init(void);
esp_err_t i2s0_set_sample_rates(uint32_t rate);
esp_err_t i2s0_dac_audio_play(short * data, int length);
esp_err_t i2s0_shdn_enable(uint8_t en);
esp_err_t i2s0_shdn_control(uint8_t count);
int i2s0_acquire_play(void);
esp_err_t i2s0_output_play(int acquire_id,short * data, int length);
esp_err_t i2s0_output_plays(short * data, int length);
#endif
