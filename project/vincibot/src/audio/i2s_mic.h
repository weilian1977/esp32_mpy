#ifndef _MIC_I2S_
#define _MIC_I2S_

#include "esp_err.h"
#include "driver/i2s.h"
#include "driver/i2c.h" 

#define ESP32_MODULE_NOT_INITIALIZE                 (0x00)
#define ESP32_MODULE_INITIALIZED                    (0x01)
#define ESP32_MODULE_DEINITIALIZED                  (0x02)
#define ESP32_MODULE_FATAL_ERROR                    (0xff)

#define CONFIG_I2S_BACK_IO_NUM                      (9)
#define CONFIG_I2S_WS_IO_NUM                        (45)
#define CONFIG_I2S_DATA_OUT_NUM                     (8)
#define CONFIG_I2S_DATA_IN_NUM                      (10)

#define CONFIG_I2S_BUFFER_COUNT                     (2)
#define CONFIG_I2S_BUFFER_SIZE                      (512)

#define CONFIG_I2S_NUM_FOR_MIC                      (0)
#define CONFIG_I2S_MIC_SAMPLE_RATE                  (16000)
#define CONFIG_I2S_MIC_SAMPLE_BITS                  (16)
#define CONFIG_I2S_MIC_READ_LEN                     (1 * 1024)
#define CONFIG_I2S_MIC_FORMAT                       (I2S_CHANNEL_FMT_ONLY_RIGHT)
#define CONFIG_I2S_MIC_CHANNEL_NUM                  ((CONFIG_I2S_MIC_FORMAT < I2S_CHANNEL_FMT_ONLY_RIGHT) ? (2) : (1))
#define CONFIG_I2S_MIC_CALCULATE_LOUDNESS_SAMPLE    (16)
#define AUDIO_RECORD_SIZE_PER_SECOND                (CONFIG_I2S_MIC_CHANNEL_NUM * CONFIG_I2S_MIC_SAMPLE_RATE * CONFIG_I2S_MIC_SAMPLE_BITS / 8)
#define I2S_MIC_NORMALIZATION_FOR_RETURN_VALUE      (327)        // 32768/327 = 100
//IO configure
#define GPIO_8218E_EN                               (GPIO_NUM_21)

#define CONFIG_I2S_MIC_TASK_STACK_SIZE              (1024 * 5)
#define ESP_I2S_MIC_TASK_PRIO                       (1)

typedef enum
{
    average_value_type = 0,
    maximum_value_type = 1,
}valume_type;

typedef enum
{
    RECORDING_STATUS_STOP = 0x00,
    RECORDING_STATUS_START,
    RECORDING_STATUS_BUFFER_FILLED,
}recording_status_t;

typedef struct
{
    int recording_status;
    uint8_t *recording_buffer;
    uint32_t buffer_len;
    uint32_t current_recording_len;
}audio_recording_structure_t;

extern esp_err_t digital_mic_init(void);
extern esp_err_t digital_mic_deinit(void);

extern esp_err_t get_loudness_value(valume_type type, float *value);

extern esp_err_t set_recording_data_buffer_t(uint8_t *buffer, uint32_t len);
extern esp_err_t get_recording_data_buffer_t(uint8_t **buffer, uint32_t *len);
extern esp_err_t get_recorded_data_len_t(uint32_t *len);
extern esp_err_t get_recording_status_t(int *sta);
extern esp_err_t start_recording_t(void);
extern esp_err_t stop_recording_t(void);

extern uint32_t get_audio_data_len_with_time_t(uint32_t time);

#endif 

