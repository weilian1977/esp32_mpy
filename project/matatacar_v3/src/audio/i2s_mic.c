#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_err.h"
#include "esp_log.h"
#include "audio_mem.h"
#include "i2s_mic.h"
#define MODULE_I2S_MIC_ENABLE 1
#if MODULE_I2S_MIC_ENABLE 
/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define TAG                                                     ("I2S_MIC")
#define MIC_READ_TASK_DELAY_TIME_MS     (25)

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/
union
{
    uint8_t byteVal[2];
    int16_t shortVal;
}val2byte;

typedef struct 
{
    int32_t  module_digital_mic_status;
    uint8_t *i2s_read_buff;
    uint16_t average_loudness_value;
    uint16_t maximum_loudness_value;
    audio_recording_structure_t audio_recording;
}digital_mic_structure_t;

/******************************************************************************
 DEFINE PRIVATE DATAS
 ******************************************************************************/
digital_mic_structure_t digital_mic_structure;

#define ESP32S3_MIC_STATUS_CHECK(a, str, ret_val) \
        if((a) != digital_mic_structure.module_digital_mic_status) \
        { \
            ESP_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str); \
            return (ret_val); \
        }

// static SemaphoreHandle_t stt_request_sem = NULL;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
static void i2s_mic_config_t(void);
static void start_i2s_mic_read_task_t(void);

static void calculate_the_loudness(uint8_t* buf, int16_t length);
static void set_audio_wr_size(uint32_t size);
static void set_stt_request_flag(void);

 /******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
esp_err_t digital_mic_init(void)
{
    if(digital_mic_structure.module_digital_mic_status == ESP32_MODULE_INITIALIZED)
    {
        return ESP_OK;
    }
    else
    {
        // stt_request_sem = xSemaphoreCreateBinary();
        // xSemaphoreGive(stt_request_sem);
        digital_mic_structure.i2s_read_buff = audio_calloc(1, CONFIG_I2S_MIC_READ_LEN);
        if(digital_mic_structure.i2s_read_buff == NULL)
        {
            ESP_LOGE(TAG, "memory alloc error");
            return ESP_FAIL;
        }
        //i2s_mic_config_t();
        start_i2s_mic_read_task_t();

        digital_mic_structure.module_digital_mic_status = ESP32_MODULE_INITIALIZED;
        return ESP_OK;
    }

}

esp_err_t digital_mic_deinit(void)
{
    return ESP_OK;
}

esp_err_t get_loudness_value(valume_type type, float *value)
{
    ESP32S3_MIC_STATUS_CHECK(ESP32_MODULE_INITIALIZED, "module not initilized", ESP_FAIL);

    if(average_value_type == type)
    {
        *value = digital_mic_structure.average_loudness_value / I2S_MIC_NORMALIZATION_FOR_RETURN_VALUE;
    }
    else
    {
        *value = digital_mic_structure.maximum_loudness_value / I2S_MIC_NORMALIZATION_FOR_RETURN_VALUE;
    }
    return ESP_OK;
}

esp_err_t set_recording_data_buffer_t(uint8_t *buffer, uint32_t len)
{
    ESP32S3_MIC_STATUS_CHECK(ESP32_MODULE_INITIALIZED, "module not initilized", ESP_FAIL);
    digital_mic_structure.audio_recording.recording_buffer = buffer;
    digital_mic_structure.audio_recording.buffer_len = len;
    return ESP_OK;
}

esp_err_t get_recording_data_buffer_t(uint8_t **buffer, uint32_t *len)
{
    ESP32S3_MIC_STATUS_CHECK(ESP32_MODULE_INITIALIZED, "module not initilized", ESP_FAIL);
    (*buffer) = digital_mic_structure.audio_recording.recording_buffer;
    (*len) = digital_mic_structure.audio_recording.buffer_len;
    return ESP_OK;
}

esp_err_t get_recorded_data_len_t(uint32_t *len)
{
    ESP32S3_MIC_STATUS_CHECK(ESP32_MODULE_INITIALIZED, "module not initilized", ESP_FAIL);    
    *len = digital_mic_structure.audio_recording.current_recording_len;

    return ESP_OK;
}

esp_err_t get_recording_status_t(int *sta)
{
    ESP32S3_MIC_STATUS_CHECK(ESP32_MODULE_INITIALIZED, "module not initilized", ESP_FAIL);    
    *sta = digital_mic_structure.audio_recording.recording_status;

    return ESP_OK;
}

esp_err_t start_recording_t(void)
{
    ESP32S3_MIC_STATUS_CHECK(ESP32_MODULE_INITIALIZED, "module not initilized", ESP_FAIL);
    digital_mic_structure.audio_recording.current_recording_len = 0;
    digital_mic_structure.audio_recording.recording_status = RECORDING_STATUS_START;
    return ESP_OK;
}

esp_err_t stop_recording_t(void)
{
    ESP32S3_MIC_STATUS_CHECK(ESP32_MODULE_INITIALIZED, "module not initilized", ESP_FAIL);
    digital_mic_structure.audio_recording.recording_status = RECORDING_STATUS_STOP;
    return ESP_OK;
}

void i2s_mic_read(void)
{
    uint32_t i2s_read_len = CONFIG_I2S_MIC_READ_LEN;
    size_t bytes_read = 0;
    size_t bytes_len_to_recording;
    i2s_read(CONFIG_I2S_NUM_FOR_MIC, (void*) digital_mic_structure.i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    calculate_the_loudness(digital_mic_structure.i2s_read_buff, i2s_read_len);
    
    // recording
    if((digital_mic_structure.audio_recording.recording_status == RECORDING_STATUS_START)
            && (digital_mic_structure.audio_recording.recording_buffer)
            && (digital_mic_structure.audio_recording.buffer_len))
    {
            bytes_len_to_recording = digital_mic_structure.audio_recording.buffer_len - digital_mic_structure.audio_recording.current_recording_len;

            bytes_read = bytes_read > bytes_len_to_recording ? bytes_len_to_recording : bytes_read;
            
            if(bytes_read)
            {
                memcpy(digital_mic_structure.audio_recording.recording_buffer + digital_mic_structure.audio_recording.current_recording_len,
                             digital_mic_structure.i2s_read_buff, bytes_read);
                digital_mic_structure.audio_recording.current_recording_len += bytes_read;
            }

            if(bytes_len_to_recording == 0)
            {
                digital_mic_structure.audio_recording.recording_status = RECORDING_STATUS_BUFFER_FILLED;
            }

            ESP_LOGD(TAG, "recording [%02d%%]\n", (digital_mic_structure.audio_recording.current_recording_len * 100) \
                                     / digital_mic_structure.audio_recording.buffer_len);
    }
}

void i2s_mic_read_task(void *parameter)
{
    while(1)
    {
        i2s_mic_read();
        if(digital_mic_structure.audio_recording.recording_status != RECORDING_STATUS_START)
        {
            vTaskDelay(MIC_READ_TASK_DELAY_TIME_MS / portTICK_RATE_MS);
        }
    }
}

uint32_t get_audio_data_len_with_time_t(uint32_t time)
{
    return AUDIO_RECORD_SIZE_PER_SECOND * time;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
static void i2s_mic_config_t(void)
{
    i2s_config_t i2s_config =
    {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX,
        .sample_rate =    CONFIG_I2S_MIC_SAMPLE_RATE,
        .bits_per_sample = CONFIG_I2S_MIC_SAMPLE_BITS,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .channel_format = CONFIG_I2S_MIC_FORMAT,
        .intr_alloc_flags = 0,
        .dma_buf_count = CONFIG_I2S_BUFFER_COUNT,
        .dma_buf_len = CONFIG_I2S_BUFFER_SIZE,
        .use_apll = true
    };
    //install and start i2s driver
    i2s_driver_install(CONFIG_I2S_NUM_FOR_MIC, &i2s_config, 0, NULL);

    i2s_pin_config_t pin_config = 
    {
            .bck_io_num = CONFIG_I2S_BACK_IO_NUM,
            .ws_io_num = CONFIG_I2S_WS_IO_NUM,
            .data_out_num = CONFIG_I2S_DATA_OUT_NUM,
            .data_in_num = CONFIG_I2S_DATA_IN_NUM     //Not used
    };
    //i2s_set_pin(CONFIG_I2S_NUM_FOR_MIC, &pin_config);
    //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);

    //// config es8218e
    //gpio_pad_select_gpio(GPIO_8218E_EN);
    //gpio_set_direction(GPIO_8218E_EN, GPIO_MODE_OUTPUT);
    //gpio_set_level(GPIO_8218E_EN, 0);
    //es8218e_start();
}

static void start_i2s_mic_read_task_t(void)
{
    /* create the task to read mic data periodicly */
#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
        static StaticTask_t i2s_mic_task_tcb;
        StackType_t *stack = audio_malloc(CONFIG_I2S_MIC_TASK_STACK_SIZE);
        xTaskCreateStaticPinnedToCore(i2s_mic_read_task, "i2s_mic_read", 
                                                                    CONFIG_I2S_MIC_TASK_STACK_SIZE, NULL, ESP_I2S_MIC_TASK_PRIO, stack, &i2s_mic_task_tcb, 0);
#else /* CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY */
        xTaskCreatePinnedToCore(i2s_mic_read_task, "i2s_mic_read",
                                                        CONFIG_I2S_MIC_TASK_STACK_SIZE, NULL, ESP_I2S_MIC_TASK_PRIO, NULL, 0);
#endif /* CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY */
}

static void calculate_the_loudness(uint8_t* buf, int16_t length)
{
    int32_t dac_value = 0;
    int32_t dac_value_addition = 0;
    int32_t average_value = 0;
    int16_t maximum_value = 0;
    for(int i = 0; i < length; i += CONFIG_I2S_MIC_CALCULATE_LOUDNESS_SAMPLE)
    {
        val2byte.byteVal[1] = buf[i + 1];
        val2byte.byteVal[0] = buf[i + 0];
        dac_value = val2byte.shortVal;
        dac_value_addition = dac_value_addition + abs(dac_value);
        if(abs(dac_value) > maximum_value)
        {
            maximum_value = abs(dac_value);
        }
    }
    average_value = CONFIG_I2S_MIC_CALCULATE_LOUDNESS_SAMPLE * dac_value_addition / length;
    digital_mic_structure.average_loudness_value = average_value;
    digital_mic_structure.maximum_loudness_value = maximum_value;
    ESP_LOGV(TAG, "mic volumn :%d", average_value);
}

#endif /* MODULE_I2S_MIC_ENABLE    */
