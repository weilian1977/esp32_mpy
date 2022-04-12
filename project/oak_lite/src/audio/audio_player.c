/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "esp_audio.h"
#include "audio_hal.h"
#include "board.h"
#include "audio_mem.h"
#include "amr_decoder.h"
#include "mp3_decoder.h"
#include "wav_decoder.h"
#include "pcm_decoder.h"

#include "audio_sonic.h"

#include "drv_i2s.h"
#include "esp_log.h"
#include "nvs_flash.h"
//#include "sdkconfig.h"

#include "http_stream.h"
#include "i2s_stream.h"
#include "vfs_stream.h"
#include "sam_stream.h"
#include "zh_hans_stream.h"

#include <string.h>
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "esp_peripherals.h"
#include "periph_sdcard.h"
//#include "drv_aw9523b.h"
#include "drv_i2c.h"
#include "render.h"
#include "sam1.h"

#include "spiffs_stream.h"
#include "periph_spiffs.h"
#include "speech_cn.h"
static const char *TAG = "audio_player.c";
#define SYNC_PLAY_MAX_TIMEOUT      30                 // 30 second
#define MAX_PATH_LENGTH            (64)
static int cur_rates = 24000;
static float  sonic_pitch = 1.0f;
static float  sonic_speed = 1.0f;
static audio_pipeline_handle_t pipeline;
static audio_element_handle_t spiffs_stream_reader,vfsfs_stream_reader, i2s_stream_writer, amr_decoder, wav_decoder, sonic_el, mp3_decoder, sam_stream_reader, zh_hans_stream_reader;
static esp_periph_set_handle_t set;
static audio_event_iface_handle_t evt;
static bool esp_audio_player_running = false;
static char path[MAX_PATH_LENGTH];
static bool esp_audio_player_rate_change = false;
STATIC void play_stop(void);

typedef struct _audio_player_obj_t
{
    mp_obj_base_t base;
    mp_obj_t callback;
    esp_audio_handle_t player;
    esp_audio_state_t state;
    audio_board_handle_t board_handle;
} audio_player_obj_t;
STATIC void audio_player_init(audio_player_obj_t *self)
{
    //i2s0_init();
    self->board_handle = audio_board_init();
    audio_hal_ctrl_codec(self->board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    periph_cfg.task_core = 0;
    set = esp_periph_set_init(&periph_cfg);
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);
    audio_hal_set_volume(self->board_handle->audio_hal, 70);
    vfs_stream_cfg_t fs_reader = VFS_STREAM_CFG_DEFAULT();
    fs_reader.type = AUDIO_STREAM_READER;
    fs_reader.task_core = 0;
    vfsfs_stream_reader = vfs_stream_init(&fs_reader);
    i2s_stream_cfg_t i2s_writer = I2S_STREAM_CFG_DEFAULT();
    i2s_writer.type = AUDIO_STREAM_WRITER;
    i2s_writer.i2s_config.sample_rate = 8000;
    i2s_writer.i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_writer.task_core = 0;
    i2s_stream_writer = i2s_stream_init(&i2s_writer);
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_cfg.task_core = 0;
    
    mp3_decoder = mp3_decoder_init(&mp3_cfg);
    // sam stream
    sam_stream_cfg_t sam_cfg = SAM_STREAM_CFG_DEFAULT();   
    sam_cfg.type = AUDIO_STREAM_READER;
    sam_cfg.task_core = 0;
    sam_stream_reader = sam_stream_init(&sam_cfg);
     // zh_hans stream
    zh_hans_stream_cfg_t zh_hans_cfg = ZH_HANS_STREAM_CFG_DEFAULT();
    zh_hans_cfg.type = AUDIO_STREAM_READER;
    zh_hans_cfg.task_core = 0;
    zh_hans_stream_reader = zh_hans_stream_init(&zh_hans_cfg);
    // amr
    amr_decoder_cfg_t amr_dec_cfg = DEFAULT_AMR_DECODER_CONFIG();
    amr_dec_cfg.task_core = 0;
    amr_decoder = amr_decoder_init(&amr_dec_cfg);

    // wav
    wav_decoder_cfg_t wav_dec_cfg = DEFAULT_WAV_DECODER_CONFIG();
    wav_dec_cfg.task_core = 0;
    wav_dec_cfg.stack_in_ext = true;
    wav_decoder = wav_decoder_init(&wav_dec_cfg);

    sonic_cfg_t sonic_cfg = DEFAULT_SONIC_CONFIG();
    sonic_cfg.sonic_info.samplerate = 16000;
    sonic_cfg.task_core = 0;
    sonic_cfg.sonic_info.channel = 1;
    sonic_cfg.sonic_info.resample_linear_interpolate = 1;
    sonic_el=sonic_init(&sonic_cfg);
    
    audio_pipeline_register(pipeline, sam_stream_reader, "sam");
    audio_pipeline_register(pipeline, vfsfs_stream_reader, "file");
    audio_pipeline_register(pipeline, zh_hans_stream_reader, "hans");
    audio_pipeline_register(pipeline, wav_decoder, "wav");
    audio_pipeline_register(pipeline, amr_decoder, "amr");
    audio_pipeline_register(pipeline, mp3_decoder, "mp3");
    audio_pipeline_register(pipeline, sonic_el, "sonic");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evt_cfg);
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

}
STATIC mp_obj_t audio_player_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    
    audio_player_obj_t *self = m_new_obj_with_finaliser(audio_player_obj_t);
    self->base.type = type;
    audio_player_init(self);
    return MP_OBJ_FROM_PTR(self);
}

void play_start(const char *uri)
{
    audio_element_info_t music_info = {0};
    esp_audio_player_running = true;
    const char *uri_p;
    char *path = strstr(uri, ":");
    char *headend = strstr(uri, ":");
    char *end;
    char head[10];
    bool get_music_info_flag = false;
    for(uri_p = uri; uri_p < headend; uri_p++)
    {
        head[uri_p - uri] = uri[uri_p - uri];
    }
    head[uri_p - uri] = 0;
    end = strrchr(uri, '.') + 1;
    if(strstr(head,"file") > 0)
    {
        const char *link_tag[3] = {head, end, "i2s"};
        audio_pipeline_link(pipeline, &link_tag[0], 3);
        audio_element_set_uri(vfsfs_stream_reader, path);
        audio_element_reset_state(mp3_decoder);
        audio_element_reset_state(wav_decoder);
        audio_element_reset_state(amr_decoder);

        get_music_info_flag = true;
    }
    else if(strstr(head,"sam") > 0)
    {
        const char *link_tag[4] = {head, end,"sonic","i2s"};
        audio_pipeline_link(pipeline, &link_tag[0], 4);
        audio_element_set_uri(sam_stream_reader, path);
        sonic_set_pitch_and_speed_info(sonic_el, sonic_pitch, sonic_speed);
    }
    else if(strstr(head,"hans") > 0)
    {
        const char *link_tag[4] = {head, end,"sonic","i2s"};
        audio_pipeline_link(pipeline, &link_tag[0], 4);
        audio_element_set_uri(zh_hans_stream_reader, path);
        sonic_set_pitch_and_speed_info(sonic_el, sonic_pitch, sonic_speed);
    }
    //i2s0_shdn_enable(0);
    if(esp_audio_player_rate_change)
    {
        
        i2s_stream_set_clk(i2s_stream_writer, cur_rates, 16, 1);
        esp_audio_player_rate_change = false;
        get_music_info_flag = false;
    }

    audio_pipeline_set_listener(pipeline, evt);
    //i2s0_shdn_enable(1);
    esp_err_t ret = audio_pipeline_run(pipeline);
    if(get_music_info_flag)
    {
        get_music_info_flag = false;
        if(strcmp(end,"mp3") == 0 || strcmp(end,"MP3") == 0 )
        {
            audio_element_getinfo(mp3_decoder, &music_info);
        }
        else if(strcmp(end,"wav") == 0 || strcmp(end,"WAV") == 0 )
        {
            audio_element_getinfo(wav_decoder, &music_info);
        }
        else if(strcmp(end,"amr") == 0 || strcmp(end,"AMR") == 0 )
        {
            audio_element_getinfo(amr_decoder, &music_info);
        }
        audio_element_setinfo(i2s_stream_writer,&music_info);
        i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
        //printf("music_info.sample_rates = %d\n,music_info.bits = %d, music_info.channels = %d\n",music_info.sample_rates,music_info.bits, music_info.channels);
    }

    if(cur_rates == 8000)
    {
        mp_hal_delay_ms(300);
    }

    AUDIO_MEM_SHOW("PRINT_MEM_RUN");
    ESP_LOGW(TAG, "audio_pipeline_run = %d\n", ret);
}
STATIC void play_stop(void)
{
    if(esp_audio_player_running == true)
    {
        esp_err_t ret = audio_pipeline_stop(pipeline);
        ESP_LOGW(TAG, "audio_pipeline_stop = %d\n", ret);
        ret = audio_pipeline_wait_for_stop(pipeline);
        ESP_LOGW(TAG, "audio_pipeline_wait_for_stop = %d\n", ret);
        ret = audio_pipeline_terminate(pipeline);
        ESP_LOGW(TAG, "audio_pipeline_terminate = %d\n", ret);
        ret = audio_pipeline_unlink(pipeline);
        ESP_LOGW(TAG, "audio_pipeline_unlink = %d\n", ret);
        i2s_stream_set_clk(i2s_stream_writer, 48000, 32, 2);
        //i2s0_shdn_enable(0);
        esp_audio_player_running = false;
    }
}

STATIC void play_wait()
{
    int count_time = 0;
    while (true)
    {
        int st = audio_element_get_state(i2s_stream_writer);
        ESP_LOGD(TAG, "play state %d\n", st);
        if(esp_audio_player_running == false)
        {
            break;
        }
        if((st != AEL_STATE_RUNNING) && (st != AEL_STATE_INIT) && (st != AEL_STATE_INITIALIZING) && (st != AEL_STATE_PAUSED))
        {          
            play_stop();
            break;
        }
        if((count_time / 50)  > SYNC_PLAY_MAX_TIMEOUT)
        {
            play_stop();
            break;
        }
        mp_hal_delay_ms(20);
        count_time++;
    }
    ESP_LOGW(TAG, "exit play_wait\n");
}

STATIC mp_obj_t audio_player_play_helper(audio_player_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    
    enum
    {
        ARG_uri,
        ARG_pos,
        ARG_sync,
        ARG_time,
    };
    static const mp_arg_t allowed_args[] =
    {
        { MP_QSTR_uri, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_pos, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_sync, MP_ARG_BOOL, { .u_bool = true } },
        { MP_QSTR_time, MP_ARG_INT, { .u_int = 0 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    //mp_obj_list_t *p = MP_OBJ_TO_PTR(rhs);
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    play_stop();
    
    mp_obj_t direct = args[ARG_uri].u_obj;
    size_t dir_length = 0;
    mp_int_t path_length = 0;
    mp_obj_t *data_ptr = NULL;
    char *file_path;
    const char *path_out = NULL;
    mp_int_t play_time = 0;
    if(n_args == 4)
    {
        play_time = args[ARG_time].u_int;

    }

    if (args[ARG_uri].u_obj != mp_const_none)
    {
        memset(path, 0, MAX_PATH_LENGTH * sizeof(char));
        const char *uri = mp_obj_str_get_str(args[ARG_uri].u_obj);
        strcat(path, uri);
        int pos = args[ARG_pos].u_int;
        ESP_LOGI(TAG, "play readly url = [%s], sync = %d\n", path, (int)args[ARG_sync].u_bool);

        file_path =  strstr(path, "/")+1;
        mp_vfs_mount_t *vfs = mp_vfs_lookup_path(file_path, &path_out);
        if (vfs == MP_VFS_NONE || vfs == MP_VFS_ROOT) {
            ESP_LOGE(TAG, "file not exist!\n");
            return mp_const_none;
        }
        if (args[ARG_sync].u_bool == false) 
        {
            play_start(path);
            //play_wait();
            return mp_const_none;
        }
        else
        {
            play_start(path);
            if(play_time)
            {
                vTaskDelay(play_time / portTICK_PERIOD_MS);
                play_stop();
            }
            else
            {
                play_wait();
            }
            return mp_const_none;
        }
    }
    else 
    {
        return mp_const_none;
    }
}

STATIC mp_obj_t audio_player_play(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    audio_player_play_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(audio_player_play_obj, 1, audio_player_play);


STATIC mp_obj_t audio_player_stop(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    play_stop();
    
    while(AEL_STATE_RUNNING == audio_element_get_state(i2s_stream_writer))
    {
        mp_hal_delay_ms(20);        
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(audio_player_stop_obj, 1, audio_player_stop);

STATIC mp_obj_t audio_player_pause(mp_obj_t self_in)
{

    if(AEL_STATE_RUNNING == audio_element_get_state(i2s_stream_writer))
    {
        audio_pipeline_pause(pipeline);
    }
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_pause_obj, audio_player_pause);

STATIC mp_obj_t audio_player_resume(mp_obj_t self_in)
{
    if(AEL_STATE_PAUSED == audio_element_get_state(i2s_stream_writer))
    {
        audio_pipeline_run(pipeline);
        audio_pipeline_resume(pipeline);
    }
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_resume_obj, audio_player_resume);

STATIC mp_obj_t audio_player_rates(mp_obj_t self_in, mp_obj_t rates)
{
    int value = mp_obj_get_int(rates);

    cur_rates = value;
    esp_audio_player_rate_change = true;
    
    return mp_obj_new_int(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(audio_player_rates_obj, audio_player_rates);

STATIC mp_obj_t audio_player_enable(mp_obj_t self_in, mp_obj_t en)
{
    int16_t value = mp_obj_get_int(en);
    i2s0_shdn_enable(value);
    return mp_obj_new_int(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(audio_player_enable_obj, audio_player_enable);


STATIC mp_obj_t audio_player_get_vol(mp_obj_t self_in)
{
    audio_player_obj_t *self = self_in;
    int vol = 0;
    audio_hal_get_volume(self->board_handle->audio_hal,&vol);
    return mp_obj_new_int(vol);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_get_vol_obj, audio_player_get_vol);

STATIC mp_obj_t audio_player_set_vol(mp_obj_t self_in, mp_obj_t vol)
{
    audio_player_obj_t *self = self_in;
    int volume = mp_obj_get_int(vol);
    return mp_obj_new_int(audio_hal_set_volume(self->board_handle->audio_hal, volume));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(audio_player_set_vol_obj, audio_player_set_vol);

STATIC mp_obj_t audio_player_say_speed(mp_obj_t self_in, mp_obj_t speed)
{
    float value = mp_obj_get_float(speed);
    sonic_speed = value + 0.0f;
    return mp_obj_new_float(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(audio_player_say_speed_obj, audio_player_say_speed);



STATIC mp_obj_t audio_player_set_mouth_throat(mp_obj_t self_in, mp_obj_t mouth,mp_obj_t throat)
{
    int mouth_in = mp_obj_get_int(mouth);
    int throat_in = mp_obj_get_int(throat);
    SetMouth(mouth_in);
    SetThroat(throat_in);
    //SetMouthThroat(mouth_in,throat_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(audio_player_set_mouth_throat_obj, audio_player_set_mouth_throat);

STATIC mp_obj_t audio_player_say_pitch(mp_obj_t self_in, mp_obj_t pitch)
{
    float value = mp_obj_get_float(pitch);
    sonic_pitch = value + 0.0f;
    return mp_obj_new_float(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(audio_player_say_pitch_obj, audio_player_say_pitch);

STATIC mp_obj_t get_audio_player_state(mp_obj_t self_in)
{
    int st = audio_element_get_state(i2s_stream_writer);
    if(st == AEL_STATE_RUNNING)
    {
        return mp_obj_new_bool(true);
    }
    if((st != AEL_STATE_RUNNING) && (st != AEL_STATE_INIT) && (st != AEL_STATE_INITIALIZING))
    {          
        //play_stop();
    }
    return mp_obj_new_bool(false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_state_obj, get_audio_player_state);

STATIC mp_obj_t audio_player_pos(mp_obj_t self_in)
{
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_pos_obj, audio_player_pos);

STATIC mp_obj_t audio_player_time(mp_obj_t self_in)
{
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_time_obj, audio_player_time);





STATIC const mp_rom_map_elem_t player_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audio_player_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audio_player_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&audio_player_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&audio_player_resume_obj) },
    { MP_ROM_QSTR(MP_QSTR_rates), MP_ROM_PTR(&audio_player_rates_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&audio_player_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_vol), MP_ROM_PTR(&audio_player_get_vol_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_vol), MP_ROM_PTR(&audio_player_set_vol_obj) },
    { MP_ROM_QSTR(MP_QSTR_say_speed), MP_ROM_PTR(&audio_player_say_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_say_pitch), MP_ROM_PTR(&audio_player_say_pitch_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_state), MP_ROM_PTR(&audio_player_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_mouth_throat), MP_ROM_PTR(&audio_player_set_mouth_throat_obj) },
    { MP_ROM_QSTR(MP_QSTR_pos), MP_ROM_PTR(&audio_player_pos_obj) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&audio_player_time_obj) },

    // esp_audio_status_t
    { MP_ROM_QSTR(MP_QSTR_STATUS_UNKNOWN), MP_ROM_INT(AUDIO_STATUS_UNKNOWN) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_RUNNING), MP_ROM_INT(AUDIO_STATUS_RUNNING) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_PAUSED), MP_ROM_INT(AUDIO_STATUS_PAUSED) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_STOPPED), MP_ROM_INT(AUDIO_STATUS_STOPPED) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_FINISHED), MP_ROM_INT(AUDIO_STATUS_FINISHED) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_ERROR), MP_ROM_INT(AUDIO_STATUS_ERROR) },

    // audio_termination_type
    { MP_ROM_QSTR(MP_QSTR_TERMINATION_NOW), MP_ROM_INT(TERMINATION_TYPE_NOW) },
    { MP_ROM_QSTR(MP_QSTR_TERMINATION_DONE), MP_ROM_INT(TERMINATION_TYPE_DONE) },
};

STATIC MP_DEFINE_CONST_DICT(player_locals_dict, player_locals_dict_table);

const mp_obj_type_t audio_player_type = {
    { &mp_type_type },
    .name = MP_QSTR_player,
    .make_new = audio_player_make_new,
    .locals_dict = (mp_obj_dict_t *)&player_locals_dict,
};