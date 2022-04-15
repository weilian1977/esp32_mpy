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

#include "errno.h"
#include <stdio.h>
#include <string.h>

#include "audio_element.h"
#include "audio_error.h"
#include "audio_mem.h"

#include "esp_log.h"
#include "sam_stream.h"
#include "wav_head.h"
#include "driver/i2s.h"

//#include "extmod/vfs_fat.h"
//#include "py/builtin.h"
//#include "py/runtime.h"
//#include "py/stream.h"
#include "sam_main.h"


static const char *TAG = "SAM_STREAM";

typedef enum {
    STREAM_TYPE_UNKNOW,
    STREAM_TYPE_WAV,
    STREAM_TYPE_OPUS,
    STREAM_TYPE_AMR,
    STREAM_TYPE_AMRWB,
} wr_stream_type_t;

typedef struct sam_stream {
    audio_stream_type_t type;
    int block_size;
    bool is_open;
    int  length;
    char *buffer;
    wr_stream_type_t w_type;
} sam_stream_t;


static char * write_data2ram(void *src,void *dst,int length)
{
    char *a=(char *)src;
    char *b=(char *)dst;
    while(length--)
    {
        *b=*a;
        b++;
        a++;
    }
    return b;
}

static char * write_wav2ram(char* buffer, int bufferlength,int64_t *l)
{
    int bufferlength1=bufferlength*2;
    int bytes_per_frame, bytes_per_sec;
    int bits_per_sample=16,sample_rate=22050,channels=1;
    bytes_per_frame = bits_per_sample/8*channels;
	bytes_per_sec   = bytes_per_frame*sample_rate;
    char *file=heap_caps_malloc(bufferlength1+50, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    char *b;
    b=write_data2ram("RIFF",file,4);
    unsigned int filesize=bufferlength1 + 4 + 8 + 16 + 8;
    b=write_data2ram(&filesize,b,4);
    b=write_data2ram("WAVE",b,4);
    b=write_data2ram("fmt ",b,4);
    unsigned int fmtlength = 16;
    b=write_data2ram(&fmtlength,b,4);
    unsigned short int format=1; 
    b=write_data2ram(&format,b,2);      // Format
    b=write_data2ram(&channels,b,2);    // Channels
    b=write_data2ram(&sample_rate,b,4);// Samplerate
    b=write_data2ram(&bytes_per_sec,b,4);// Bytes per sec
    b=write_data2ram(&bytes_per_frame,b,2);// Bytes per frame
    b=write_data2ram(&bits_per_sample,b,2);// Bits per sample
    b=write_data2ram("data",b,4);
    b=write_data2ram(&bufferlength1,b,4);
    for(int i=0;i<bufferlength;i++)
    {
        short t=((short)buffer[i]-128)*200;
        *b=t&0xff;b++;
        *b=t>>8;b++;
    }
    heap_caps_free(buffer);
    *l=(b-file);
    return file;
}
static esp_err_t _sam_open(audio_element_handle_t self)
{
    sam_stream_t *sam = (sam_stream_t *)audio_element_getdata(self);

    audio_element_info_t info;
    char *uri = audio_element_get_uri(self);
    if (uri == NULL) {
        ESP_LOGE(TAG, "Error, uri is not set");
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "_sam_open, uri:%s", uri);
//    char *path = strstr(uri, "/sdcard");
    char *path = strstr(uri, "/")+1;
    audio_element_getinfo(self, &info);
    if (path == NULL) {
        ESP_LOGE(TAG, "Error, need file path to open");
        return ESP_FAIL;
    }
    if (sam->is_open) {
        ESP_LOGE(TAG, "already opened");
        return ESP_FAIL;
    }
    if (sam->type == AUDIO_STREAM_READER) {
        char context[120];char *ct_p;
        ct_p=context;
        path+=1;
        while(*path)
        {
            *ct_p=*path;
            ct_p++;
            path++;
        }
        ct_p--;
        while(*ct_p)
        {
            if(*ct_p=='/')break;
            ct_p--;
        }
        *ct_p=0;
        printf("play text=%s\n",context);
        sam->buffer=heap_caps_malloc(SAM_USER_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        info.total_bytes=sam_translate(context,sam->buffer,SAM_USER_BUF_SIZE);
        if(info.total_bytes>=SAM_USER_BUF_SIZE)
        {
            info.total_bytes=SAM_USER_BUF_SIZE;
            printf("info.total_bytes=The buffer out size\n");
        }
        sam->buffer=write_wav2ram(sam->buffer,info.total_bytes,&info.total_bytes);
        sam->length=SAM_USER_BUF_SIZE;
        //ESP_LOGI(TAG, "File size is %d byte,pos:%d", (int)info.total_bytes, (int)info.byte_pos);
    } 
    sam->is_open = true;

    return audio_element_setinfo(self, &info);
}

static int _sam_read(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    sam_stream_t *sam = (sam_stream_t *)audio_element_getdata(self);
    audio_element_info_t info;
    audio_element_getinfo(self, &info);
    int rlen;
    if(len<(info.total_bytes-info.byte_pos))rlen=len;
    else rlen=info.total_bytes-info.byte_pos;
    if(info.total_bytes<info.byte_pos)rlen=0;
    for(int i=0;i<rlen;i++)buffer[i]=sam->buffer[i+info.byte_pos];
    if (rlen <= 0) {
        ESP_LOGW(TAG, "No more data,ret:%d", rlen);
        rlen = 0;
    } else {
        info.byte_pos += rlen;
        audio_element_setinfo(self, &info);
    }
    return rlen;
}

static int _sam_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    int r_size = audio_element_input(self, in_buffer, in_len);
    int w_size = 0;
    if (r_size > 0) {
        w_size = audio_element_output(self, in_buffer, r_size);
    } else {
        w_size = r_size;
    }
    return w_size;
}

static esp_err_t _sam_close(audio_element_handle_t self)
{
    sam_stream_t *sam = (sam_stream_t *)audio_element_getdata(self);
    
    if (sam->is_open) {
        heap_caps_free(sam->buffer);
        sam->is_open = false;
    }
    if (AEL_STATE_PAUSED != audio_element_get_state(self)) {
        audio_element_report_info(self);
        audio_element_info_t info = { 0 };
        audio_element_getinfo(self, &info);
        info.byte_pos = 0;
        audio_element_setinfo(self, &info);
    }
    return ESP_OK;
}

static esp_err_t _sam_destroy(audio_element_handle_t self)
{
    sam_stream_t *sam = (sam_stream_t *)audio_element_getdata(self);
    audio_free(sam);
    return ESP_OK;
}

audio_element_handle_t sam_stream_init(sam_stream_cfg_t *config)
{
    audio_element_handle_t el;
    sam_stream_t *sam = audio_calloc(1, sizeof(sam_stream_t));

    AUDIO_MEM_CHECK(TAG, sam, return NULL);

    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.open = _sam_open;
    cfg.close = _sam_close;
    cfg.process = _sam_process;
    cfg.destroy = _sam_destroy;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.buffer_len = config->buf_sz;
    if (cfg.buffer_len == 0) {
        cfg.buffer_len = SAM_STREAM_BUF_SIZE;
    }

    cfg.tag = "sam";
    sam->type = config->type;
    cfg.read = _sam_read;
    el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, goto _sam_init_exit);
    audio_element_setdata(el, sam);
    return el;
_sam_init_exit:
    audio_free(sam);
    return NULL;
}
