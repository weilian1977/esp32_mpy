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

#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "audio_hal.h"
#include "audio_pipeline.h"
#include "board.h"

#include "i2s_stream.h"
#include "vfs_stream.h"

#include "amrnb_encoder.h"

#include "esp_err.h"
#include "esp_log.h"
#include "audio_element.h"

#include "audio_record.h"

static const char *TAG = "audio_recorder.c";

#define SYNC_RECORDER_MAX_TIMEOUT      30
static esp_periph_set_handle_t set;
static audio_event_iface_handle_t evt;
typedef struct _audio_recorder_obj_t
{
    mp_obj_base_t base;

    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_stream;
    audio_element_handle_t encoder;
    audio_element_handle_t out_stream;

    esp_timer_handle_t timer;
    mp_obj_t end_cb;
} audio_recorder_obj_t;
audio_recorder_obj_t recorder_audio_init;

static bool esp_audio_recorder_running = false;

void recorder_stop(void)
{
    if(esp_audio_recorder_running == true)
    {
        printf("recorder stop start\n");
        esp_audio_recorder_running = false; 
        audio_pipeline_stop(recorder_audio_init.pipeline);
        audio_pipeline_wait_for_stop(recorder_audio_init.pipeline);
        audio_pipeline_terminate(recorder_audio_init.pipeline);
        audio_pipeline_unlink(recorder_audio_init.pipeline);
        ESP_LOGW(TAG, "audio_recorder_stop \n");
        //i2s_stream_set_clk(recorder_audio_init.i2s_stream, 48000, 32, 2);
    }
}
STATIC mp_obj_t audio_recorder_stop(mp_obj_t self_in);
void audio_recorder_init(void)
{
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);

    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    periph_cfg.task_core = 0;
    set = esp_periph_set_init(&periph_cfg);
    // pipeline
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    recorder_audio_init.pipeline = audio_pipeline_init(&pipeline_cfg);
    // I2S
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_READER;
    i2s_cfg.uninstall_drv = false;
    i2s_cfg.i2s_config.sample_rate = 8000;
    i2s_cfg.i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT;
    i2s_cfg.task_core = 0;
    i2s_cfg.task_prio = 23;
    recorder_audio_init.i2s_stream = i2s_stream_init(&i2s_cfg);

    // encoder
    amrnb_encoder_cfg_t amr_enc_cfg = DEFAULT_AMRNB_ENCODER_CONFIG();
    amr_enc_cfg.task_core = 0;
    amr_enc_cfg.out_rb_size = 2 * 1024;
    recorder_audio_init.encoder = amrnb_encoder_init(&amr_enc_cfg);

    // out stream
    vfs_stream_cfg_t vfs_cfg = VFS_STREAM_CFG_DEFAULT();
    vfs_cfg.type = AUDIO_STREAM_WRITER;
    vfs_cfg.task_core = 0;
    vfs_cfg.task_prio = 4;
    recorder_audio_init.out_stream = vfs_stream_init(&vfs_cfg);
    // register to pipeline
    audio_pipeline_register(recorder_audio_init.pipeline, recorder_audio_init.i2s_stream, "i2s");
    audio_pipeline_register(recorder_audio_init.pipeline, recorder_audio_init.encoder, "encoder");
    audio_pipeline_register(recorder_audio_init.pipeline, recorder_audio_init.out_stream, "out");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evt_cfg);
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);
    ESP_LOGW(TAG, "audio_pipeline_init done");
}
STATIC mp_obj_t audio_recorder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    audio_recorder_obj_t *self = m_new_obj_with_finaliser(audio_recorder_obj_t);
    self->base.type = type;
    return MP_OBJ_FROM_PTR(self);
}

int timer_recorder = 0;
STATIC mp_obj_t audio_recorder_start(mp_uint_t n_args, const mp_obj_t *args_in, mp_map_t *kw_args)
{
    enum 
    {
        ARG_uri,
        ARG_maxtime,
        ARG_endcb
    };
    static const mp_arg_t allowed_args[] = 
    {
        { MP_QSTR_uri, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_maxtime, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_endcb, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    recorder_stop();
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    i2s_stream_set_clk(recorder_audio_init.i2s_stream, 8000, 16, 1);
    mp_arg_parse_all(n_args - 1, args_in + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int time_count = 0;
    ESP_LOGW(TAG, "audio_recorder_start");
    const char *link_tag[3] = {"i2s", "encoder", "out"};
    audio_pipeline_link(recorder_audio_init.pipeline, &link_tag[0], 3);
    audio_element_set_uri(recorder_audio_init.out_stream, mp_obj_str_get_str(args[ARG_uri].u_obj));
    audio_pipeline_set_listener(recorder_audio_init.pipeline, evt);

    if (audio_pipeline_run(recorder_audio_init.pipeline) == ESP_OK) 
    {
        esp_audio_recorder_running = true;
        if (args[ARG_maxtime].u_int > 0) 
        {
            timer_recorder = args[ARG_maxtime].u_int;
            while (1) {
                int st = audio_element_get_state(recorder_audio_init.i2s_stream);
                if(st != AEL_STATE_RUNNING)
                {
                    audio_element_set_ringbuf_done(recorder_audio_init.i2s_stream);
                    break;
                }
                mp_hal_delay_ms(1000);
            //audio_event_iface_msg_t msg;
                time_count ++;
                ESP_LOGI(TAG, "[ * ] Recording ... %d", time_count);
                if (time_count < timer_recorder) {
                    continue;
                    
                }
                audio_element_set_ringbuf_done(recorder_audio_init.i2s_stream);
                break;
            }
        esp_audio_recorder_running = false;
        printf("recorder stop start\n");
        audio_pipeline_stop(recorder_audio_init.pipeline);
        audio_pipeline_wait_for_stop(recorder_audio_init.pipeline);
        audio_pipeline_terminate(recorder_audio_init.pipeline);
        audio_pipeline_unlink(recorder_audio_init.pipeline);
        ESP_LOGW(TAG, "audio_recorder_stop \n");
        //i2s_stream_set_clk(recorder_audio_init.i2s_stream, 48000, 32, 2);
        }
        return mp_obj_new_bool(true);
    }
    else 
    {
        ESP_LOGW(TAG, "recorder audio_pipeline_run error");
        esp_audio_recorder_running = false;
        return mp_obj_new_bool(false);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(audio_recorder_start_obj, 1, audio_recorder_start);

STATIC mp_obj_t audio_recorder_stop(mp_obj_t self_in)
{
    if(esp_audio_recorder_running == true)
    {
        printf("recorder stop start\n");
        esp_audio_recorder_running = false;     
        audio_pipeline_stop(recorder_audio_init.pipeline);
        audio_pipeline_wait_for_stop(recorder_audio_init.pipeline);
        audio_pipeline_terminate(recorder_audio_init.pipeline);
        audio_pipeline_unlink(recorder_audio_init.pipeline);
        ESP_LOGW(TAG, "audio_recorder_stop \n");
        //i2s_stream_set_clk(recorder_audio_init.i2s_stream, 48000, 32, 2);
    } 
    return mp_obj_new_bool(true);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_recorder_stop_obj, audio_recorder_stop);

STATIC mp_obj_t audio_recorder_is_running(mp_obj_t self_in)
{
    return mp_obj_new_bool(esp_audio_recorder_running);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_recorder_is_running_obj, audio_recorder_is_running);

STATIC const mp_rom_map_elem_t recorder_locals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&audio_recorder_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audio_recorder_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_running), MP_ROM_PTR(&audio_recorder_is_running_obj) },
};

STATIC MP_DEFINE_CONST_DICT(recorder_locals_dict, recorder_locals_dict_table);

const mp_obj_type_t audio_recorder_type =
{
    { &mp_type_type },
    .name = MP_QSTR_recorder,
    .make_new = audio_recorder_make_new,
    .locals_dict = (mp_obj_dict_t *)&recorder_locals_dict,
};
