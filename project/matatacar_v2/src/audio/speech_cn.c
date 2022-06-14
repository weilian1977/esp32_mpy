#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "esp_log.h"
#include "audio_tone_uri.h"
#include "amrnb_encoder.h"
#include "audio_element.h"
#include "audio_idf_version.h"
#include "audio_mem.h"
#include "audio_pipeline.h"
#include "audio_recorder.h"
#include "audio_thread.h"
#include "board.h"
#include "esp_audio.h"
#include "filter_resample.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"
#include "periph_adc_button.h"
#include "raw_stream.h"
#include "recorder_encoder.h"
#include "recorder_sr.h"
#include "tone_stream.h"

#include "model_path.h"
#include "speech_cn.h"
#include "audio_mem.h"

#define RECORDER_ENC_ENABLE (false)
#define VOICE2FILE          (false)
#define WAKENET_ENABLE      (true)

#define MAX_COMMANDS_ID 99
#ifdef CONFIG_ESP_LYRAT_MINI_V1_1_BOARD
#define RECORDER_SAMPLE_RATE (16000)
#else
#define RECORDER_SAMPLE_RATE (48000)
#endif


#ifndef CODEC_ADC_SAMPLE_RATE
#warning "Please define CODEC_ADC_SAMPLE_RATE first, default value is 48kHz may not correctly"
#define CODEC_ADC_SAMPLE_RATE    48000
#endif
#ifndef CODEC_ADC_BITS_PER_SAMPLE
#warning "Please define CODEC_ADC_BITS_PER_SAMPLE first, default value 16 bits may not correctly"
#define CODEC_ADC_BITS_PER_SAMPLE  I2S_BITS_PER_SAMPLE_16BIT
#endif
#ifndef RECORD_HARDWARE_AEC
#warning "The hardware AEC is disabled!"
#define RECORD_HARDWARE_AEC  (false)
#endif
#ifndef CODEC_ADC_I2S_PORT
#define CODEC_ADC_I2S_PORT  (0)
#endif
enum _rec_msg_id {
    REC_START = 1,
    REC_STOP,
    REC_CANCEL,
};
static char *TAG = "speech_recognition";
static bool esp_audio_recorder_running = true;
static esp_audio_handle_t     player        = NULL;
static audio_rec_handle_t     recorder      = NULL;
static audio_element_handle_t raw_read      = NULL;
static QueueHandle_t          rec_q         = NULL;
static bool                   voice_reading = false;
static audio_rec_cfg_t cfg = AUDIO_RECORDER_DEFAULT_CFG();
static esp_periph_set_handle_t set_recorder;
char *ch_commands_str = NULL;
static int speech_cmd_id = -1;
static int commands_index = 0;
static char *commands_str[MAX_COMMANDS_ID];
void separate_commands(char *all_commands)
{
    
    AUDIO_MEM_SHOW("TAG1");
    char * tmp;
    char * str;

    //memset(commands_str[commands_index], 0x00, 64);
    commands_index = 0;
    if(commands_str[commands_index] != NULL)
        free(commands_str[commands_index]);
    commands_str[commands_index] = (char *)malloc(64);
    str = strtok_r(all_commands, ";", &tmp);
    strcpy(commands_str[commands_index],str);
    //memcpy(commands_str[commands_index],str,strlen(str));
    while(tmp!=NULL)
    {
        str = strtok_r(0, ";", &tmp);
        if(str == NULL)
        {
            return;
        }
        commands_index ++;
        if(commands_str[commands_index] != NULL)
            free(commands_str[commands_index]);
        //memset(commands_str[commands_index], 0x00, 64);
        commands_str[commands_index] = (char *)malloc(64);
        strcpy(commands_str[commands_index],str);
        //memcpy(commands_str[commands_index],str,strlen(str));
    }
}


static esp_audio_handle_t setup_player()
{
    esp_audio_cfg_t cfg = DEFAULT_ESP_AUDIO_CONFIG();
    audio_board_handle_t board_handle = audio_board_init();

    cfg.vol_handle = board_handle->audio_hal;
    cfg.vol_set = (audio_volume_set)audio_hal_set_volume;
    cfg.vol_get = (audio_volume_get)audio_hal_get_volume;
    cfg.resample_rate = 48000;
    cfg.prefer_type = ESP_AUDIO_PREFER_MEM;

    player = esp_audio_create(&cfg);
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);

    // Create readers and add to esp_audio
    tone_stream_cfg_t tone_cfg = TONE_STREAM_CFG_DEFAULT();
    tone_cfg.type = AUDIO_STREAM_READER;
    esp_audio_input_stream_add(player, tone_stream_init(&tone_cfg));

    // Add decoders and encoders to esp_audio
    mp3_decoder_cfg_t mp3_dec_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_dec_cfg.task_core = 1;
    esp_audio_codec_lib_add(player, AUDIO_CODEC_TYPE_DECODER, mp3_decoder_init(&mp3_dec_cfg));

    // Create writers and add to esp_audio
    i2s_stream_cfg_t i2s_writer = I2S_STREAM_CFG_DEFAULT();
    i2s_writer.i2s_config.sample_rate = 48000;
    i2s_writer.i2s_config.bits_per_sample = CODEC_ADC_BITS_PER_SAMPLE;
    i2s_writer.need_expand = (CODEC_ADC_BITS_PER_SAMPLE != I2S_BITS_PER_SAMPLE_16BIT);
    i2s_writer.type = AUDIO_STREAM_WRITER;

    esp_audio_output_stream_add(player, i2s_stream_init(&i2s_writer));

    // Set default volume
    esp_audio_vol_set(player, 70);
    AUDIO_MEM_SHOW(TAG);

    ESP_LOGI(TAG, "esp_audio instance is:%p\r\n", player);
    return player;
}

#if VOICE2FILE == (true)
static void voice_2_file(uint8_t *buffer, int len)
{
#define MAX_FNAME_LEN (50)

    static FILE *fp = NULL;
    static int fcnt = 0;

    if (voice_reading) {
        if (!fp) {
            if (fp == NULL) {
                char fname[MAX_FNAME_LEN] = { 0 };

                if (RECORDER_ENC_ENABLE) {
                    snprintf(fname, MAX_FNAME_LEN - 1, "/f%d.amr", fcnt++);
                } else {
                    snprintf(fname, MAX_FNAME_LEN - 1, "/f%d.pcm", fcnt++);
                }
                ESP_LOGE(TAG, "File open start");
                printf("fname = %s\n",fname);
                fp = fopen(fname, "wb");
                
                ESP_LOGE(TAG, "File open over");
                if (!fp) {
                    ESP_LOGE(TAG, "File open failed");
                }
            }
        }
        if (len) {
            fwrite(buffer, len, 1, fp);
        }
    } else {
        if (fp) {
            ESP_LOGI(TAG, "File closed");
            fclose(fp);
            fp = NULL;
        }
    }
}
#endif /* VOICE2FILE == (true) */
void voice_read_task()
{

    static bool init_flag = true;
    static const int buf_len = 2 * 1024;
    static uint8_t *voiceData;
    static int msg = 0;
    static TickType_t delay = 0;
    
    if(init_flag == true)
    {
        init_flag = false;
        voiceData = audio_calloc(1, buf_len);
    }
    if (xQueueReceive(rec_q, &msg, delay) == pdTRUE) {
        switch (msg) {
            case REC_START: {
                ESP_LOGW(TAG, "voice read begin");
                voice_reading = true;
                break;
            }
            case REC_STOP: {
                ESP_LOGW(TAG, "voice read stopped");
                voice_reading = false;
                break;
            }
            case REC_CANCEL: {
                ESP_LOGW(TAG, "voice read cancel");
                voice_reading = false;
                break;
            }
            default:
                break;
        }
    }
    int ret = 0;
    if (voice_reading) {
        ret = audio_recorder_data_read(recorder, voiceData, buf_len, portMAX_DELAY);
        if (ret <= 0) {
            ESP_LOGW(TAG, "audio recorder read finished %d", ret);
            voice_reading = false;
        }
    }
#if VOICE2FILE == (true)
        voice_2_file(voiceData, ret);
#endif /* VOICE2FILE == (true) */
}

void voice_read_task_c()
{
//    speech_cn_init();
    const int buf_len = 2 * 1024;
    uint8_t *voiceData = audio_calloc(1, buf_len);
    int msg = 0;
    TickType_t delay = portMAX_DELAY;
    while (true) {
        if (xQueueReceive(rec_q, &msg, delay) == pdTRUE) {
            switch (msg) {
                case REC_START: {
                    ESP_LOGW(TAG, "voice read begin");
                    delay = 0;
                    voice_reading = true;
                    break;
                }
                case REC_STOP: {
                    ESP_LOGW(TAG, "voice read stopped");
                    delay = portMAX_DELAY;
                    voice_reading = false;
                    break;
                }
                case REC_CANCEL: {
                    ESP_LOGW(TAG, "voice read cancel");
                    delay = portMAX_DELAY;
                    voice_reading = false;
                    break;
                }
                default:
                    break;
            }
        }
        int ret = 0;
        if (voice_reading) {
            ret = audio_recorder_data_read(recorder, voiceData, buf_len, portMAX_DELAY);
            if (ret <= 0) {
                ESP_LOGW(TAG, "audio recorder read finished %d", ret);
                delay = portMAX_DELAY;
                voice_reading = false;
            }
        }
#if VOICE2FILE == (true)
    voice_2_file(voiceData, ret);
#endif /* VOICE2FILE == (true) */
    }

    free(voiceData);
    vTaskDelete(NULL);
}

static esp_err_t rec_engine_cb(audio_rec_evt_t type, void *user_data)
{
    if (AUDIO_REC_WAKEUP_START == type) {
        ESP_LOGI(TAG, "rec_engine_cb - REC_EVENT_WAKEUP_START");
        esp_audio_sync_play(player, tone_uri[TONE_TYPE_DINGDONG], 0);
        if (voice_reading) {
            int msg = REC_CANCEL;
            if (xQueueSend(rec_q, &msg, 0) != pdPASS) {
                //ESP_LOGE(TAG, "rec cancel send failed");
            }
        }
    } else if (AUDIO_REC_VAD_START == type) {
        ESP_LOGI(TAG, "rec_engine_cb - REC_EVENT_VAD_START");
        if (!voice_reading) {
            int msg = REC_START;
            if (xQueueSend(rec_q, &msg, 0) != pdPASS) {
                //ESP_LOGE(TAG, "rec start send failed");
            }
        }
    } else if (AUDIO_REC_VAD_END == type) {
        ESP_LOGI(TAG, "rec_engine_cb - REC_EVENT_VAD_STOP");
        if (voice_reading) {
            int msg = REC_STOP;
            if (xQueueSend(rec_q, &msg, 0) != pdPASS) {
                //ESP_LOGE(TAG, "rec stop send failed");
            }
        }

    } else if (AUDIO_REC_WAKEUP_END == type) {
        ESP_LOGI(TAG, "rec_engine_cb - REC_EVENT_WAKEUP_END");
    } else if (AUDIO_REC_COMMAND_DECT <= type) {
        speech_cmd_id = type;
        //ESP_LOGI(TAG, "rec_engine_cb - AUDIO_REC_COMMAND_DECT");
        ESP_LOGW(TAG, "command %d", type);
        //esp_audio_sync_play(player, tone_uri[TONE_TYPE_HAODE], 0);

    } else {
        ESP_LOGE(TAG, "Unkown event");
    }
    return ESP_OK;
}

static int input_cb_for_afe(int16_t *buffer, int buf_sz, void *user_ctx, TickType_t ticks)
{
    return raw_stream_read(raw_read, (char *)buffer, buf_sz);
}

static void start_recorder()
{
    srmodel_spiffs_init();
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline_rec = audio_pipeline_init(&pipeline_cfg);
    if (NULL == pipeline_rec) {
        return;
    }

    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.i2s_port = CODEC_ADC_I2S_PORT;
    i2s_cfg.i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT;
    i2s_cfg.i2s_config.use_apll = 0;
    i2s_cfg.i2s_config.sample_rate = CODEC_ADC_SAMPLE_RATE;
    i2s_cfg.i2s_config.bits_per_sample = CODEC_ADC_BITS_PER_SAMPLE;
    i2s_cfg.type = AUDIO_STREAM_READER;
    i2s_stream_reader = i2s_stream_init(&i2s_cfg);

    audio_element_handle_t filter = NULL;
#if CODEC_ADC_SAMPLE_RATE != (16000)
    rsp_filter_cfg_t rsp_cfg = DEFAULT_RESAMPLE_FILTER_CONFIG();
    rsp_cfg.src_rate = CODEC_ADC_SAMPLE_RATE;
    rsp_cfg.dest_rate = 16000;
    filter = rsp_filter_init(&rsp_cfg);
#endif

    raw_stream_cfg_t raw_cfg = RAW_STREAM_CFG_DEFAULT();
    raw_cfg.type = AUDIO_STREAM_READER;
    raw_read = raw_stream_init(&raw_cfg);

    audio_pipeline_register(pipeline_rec, i2s_stream_reader, "i2s_recoder");
    audio_pipeline_register(pipeline_rec, raw_read, "raw");
    if (filter) {
        audio_pipeline_register(pipeline_rec, filter, "filter");
        const char *link_tag[3] = {"i2s_recoder", "filter", "raw"};
        audio_pipeline_link(pipeline_rec, &link_tag[0], 3);
    } else {
        const char *link_tag[2] = {"i2s_recoder", "raw"};
        audio_pipeline_link(pipeline_rec, &link_tag[0], 2);
    }

    ESP_LOGI(TAG, "[ 4 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg_recorder = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt_recorder = audio_event_iface_init(&evt_cfg_recorder);

    ESP_LOGI(TAG, "[4.1] Listening event from all elements of pipeline_rec");
    audio_pipeline_set_listener(pipeline_rec, evt_recorder);

    ESP_LOGI(TAG, "[4.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set_recorder), evt_recorder);

    audio_pipeline_run(pipeline_rec);
    ESP_LOGI(TAG, "Recorder has been created");
    recorder_sr_cfg_t recorder_sr_cfg = DEFAULT_RECORDER_SR_CFG();
    recorder_sr_cfg.afe_cfg.alloc_from_psram = 3;
    recorder_sr_cfg.afe_cfg.wakenet_init = WAKENET_ENABLE;
    recorder_sr_cfg.afe_cfg.aec_init = RECORD_HARDWARE_AEC;
#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0))
    recorder_sr_cfg.input_order[0] = DAT_CH_REF0;
    recorder_sr_cfg.input_order[1] = DAT_CH_0;
#endif
#if RECORDER_ENC_ENABLE == (true)
    rsp_filter_cfg_t filter_cfg = DEFAULT_RESAMPLE_FILTER_CONFIG();
    filter_cfg.src_ch = 1;
    filter_cfg.src_rate = 16000;
    filter_cfg.dest_ch = 1;
    filter_cfg.dest_rate = 8000;
    filter_cfg.stack_in_ext = true;
    filter_cfg.max_indata_bytes = 1024;

    amrnb_encoder_cfg_t amrnb_cfg = DEFAULT_AMRNB_ENCODER_CONFIG();
    amrnb_cfg.contain_amrnb_header = true;
    amrnb_cfg.stack_in_ext = true;

    recorder_encoder_cfg_t recorder_encoder_cfg = { 0 };
    recorder_encoder_cfg.resample = rsp_filter_init(&filter_cfg);
    recorder_encoder_cfg.encoder = amrnb_encoder_init(&amrnb_cfg);
#endif
    //audio_rec_cfg_t cfg = AUDIO_RECORDER_DEFAULT_CFG();
    cfg.read = (recorder_data_read_t)&input_cb_for_afe;
    cfg.sr_handle = recorder_sr_create(&recorder_sr_cfg, &cfg.sr_iface);
#if RECORDER_ENC_ENABLE == (true)
    cfg.encoder_handle = recorder_encoder_create(&recorder_encoder_cfg, &cfg.encoder_iface);
#endif
    cfg.event_cb = rec_engine_cb;
    cfg.vad_off = 1000;

}

static void log_clear(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);

    esp_log_level_set("AUDIO_THREAD", ESP_LOG_ERROR);
    esp_log_level_set("I2C_BUS", ESP_LOG_ERROR);
    esp_log_level_set("AUDIO_HAL", ESP_LOG_ERROR);
    esp_log_level_set("ESP_AUDIO_TASK", ESP_LOG_ERROR);
    esp_log_level_set("ESP_DECODER", ESP_LOG_ERROR);
    esp_log_level_set("I2S", ESP_LOG_ERROR);
    esp_log_level_set("AUDIO_FORGE", ESP_LOG_ERROR);
    esp_log_level_set("ESP_AUDIO_CTRL", ESP_LOG_ERROR);
    esp_log_level_set("AUDIO_PIPELINE", ESP_LOG_ERROR);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_ERROR);
    esp_log_level_set("TONE_PARTITION", ESP_LOG_ERROR);
    esp_log_level_set("TONE_STREAM", ESP_LOG_ERROR);
    esp_log_level_set("MP3_DECODER", ESP_LOG_ERROR);
    esp_log_level_set("I2S_STREAM", ESP_LOG_ERROR);
    esp_log_level_set("RSP_FILTER", ESP_LOG_ERROR);
    esp_log_level_set("AUDIO_EVT", ESP_LOG_ERROR);
}

void speech_cn_init(void)
{
    ch_commands_str = (char *)malloc(64*99);
    strcpy(ch_commands_str, "ni hao");

    log_clear();
    esp_periph_config_t periph_cfg_recorder = DEFAULT_ESP_PERIPH_SET_CONFIG();
    periph_cfg_recorder.extern_stack = true;
    set_recorder = esp_periph_set_init(&periph_cfg_recorder);
    audio_board_init();
    start_recorder();
    setup_player();
    rec_q = xQueueCreate(3, sizeof(int));
    char err[200];
    recorder_sr_reset_speech_cmd(cfg.sr_handle, ch_commands_str, err);
    separate_commands(ch_commands_str);
    recorder = audio_recorder_create(&cfg);
    if(pipeline_rec == NULL)
        return;
    if(esp_audio_recorder_running == true)
    {
        
        recorder_sr_enable(cfg.sr_handle,false);
        audio_pipeline_stop(pipeline_rec);
        audio_pipeline_wait_for_stop(pipeline_rec);
        audio_pipeline_terminate(pipeline_rec);
        audio_pipeline_unlink(pipeline_rec);
        ESP_LOGW(TAG, "stop_speech_recognition\n");
        esp_audio_recorder_running = false;
    }
}


STATIC mp_obj_t change_commid(mp_obj_t commands)
{
    const char *commands_str = mp_obj_str_get_str(commands);
    char err[200];
    memset(ch_commands_str, 0x00, 64*99);
    strcpy(ch_commands_str, commands_str);
    recorder_sr_reset_speech_cmd(cfg.sr_handle, ch_commands_str, err);
    separate_commands(ch_commands_str);

    return mp_const_none;
}

STATIC mp_obj_t is_speech_commands(mp_obj_t command)
{
    const char *read_command = mp_obj_str_get_str(command);
    if(speech_cmd_id == -1)
    {
        return mp_obj_new_bool(false);
    }
    else
    {
        if(strcmp(read_command,commands_str[speech_cmd_id])==0)
        {
            speech_cmd_id = -1;
            return mp_obj_new_bool(true);
        }
        else
        {
            return mp_obj_new_bool(false);
        }
    }
    return mp_obj_new_bool(false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_is_speech_commands_obj, is_speech_commands);

STATIC mp_obj_t start_speech_recognition()
{
    if(pipeline_rec == NULL)
        return mp_const_none;
    if(esp_audio_recorder_running == false)
    {
        printf("start_speech\n");
        i2s_stream_set_clk(i2s_stream_reader, 48000, 16, 1);
        const char *link_tag[3] = {"i2s_recoder", "filter", "raw"};
        audio_pipeline_link(pipeline_rec, &link_tag[0], 3);
        audio_pipeline_run(pipeline_rec);
        recorder_sr_enable(cfg.sr_handle,true);
        ESP_LOGW(TAG, "start_speech_recognition\n");
        esp_audio_recorder_running = true;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_start_speech_recognition_obj, start_speech_recognition);
void speech_stop()
{
    if(pipeline_rec == NULL)
        return ;
    if(esp_audio_recorder_running == true)
    {
        recorder_sr_enable(cfg.sr_handle,false);
        audio_pipeline_stop(pipeline_rec);
        audio_pipeline_wait_for_stop(pipeline_rec);
        audio_pipeline_terminate(pipeline_rec);
        audio_pipeline_unlink(pipeline_rec);
        ESP_LOGW(TAG, "stop_speech_recognition\n");
        esp_audio_recorder_running = false;
    }
}
STATIC mp_obj_t stop_speech_recognition()
{
    if(pipeline_rec == NULL)
        return mp_const_none;
    if(esp_audio_recorder_running == true)
    {
        recorder_sr_enable(cfg.sr_handle,false);
        audio_pipeline_stop(pipeline_rec);
        audio_pipeline_wait_for_stop(pipeline_rec);
        audio_pipeline_terminate(pipeline_rec);
        audio_pipeline_unlink(pipeline_rec);
        ESP_LOGW(TAG, "stop_speech_recognition\n");
        esp_audio_recorder_running = false;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_stop_speech_recognition_obj, stop_speech_recognition);

STATIC mp_obj_t speech_recognition()
{
    voice_read_task();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_speech_recognition_obj, speech_recognition);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_change_commid_obj, change_commid);

STATIC const mp_map_elem_t speech_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_speech_recognition),                    (mp_obj_t)&mpy_speech_recognition_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_change_commid),                         (mp_obj_t)&mpy_change_commid_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_speech_commands),                    (mp_obj_t)&mpy_is_speech_commands_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_start_speech_recognition),              (mp_obj_t)&mpy_start_speech_recognition_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stop_speech_recognition),               (mp_obj_t)&mpy_stop_speech_recognition_obj },


};

STATIC MP_DEFINE_CONST_DICT(speech_module_globals, speech_module_globals_table);

const mp_obj_module_t speech_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&speech_module_globals,
};
