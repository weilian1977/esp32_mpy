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
#include "zh_hans_stream.h"
#include "wav_head.h"

//#include "extmod/vfs_fat.h"
//#include "py/builtin.h"
//#include "py/runtime.h"
//#include "py/stream.h"
#include "zh_hans_stream.h"

#include "esp_spi_flash.h"
#include "esp32/spiram.h"
#include "esp_partition.h"

#include "esp_tts.h"
#include "esp_tts_voice_xiaole.h"
#include "esp_tts_voice_template.h"
#include "esp_tts_player.h"

static const char *TAG = "ZH_HANS_STREAM";
static esp_tts_handle_t *tts_handle;
typedef enum {
    STREAM_TYPE_UNKNOW,
    STREAM_TYPE_WAV,
    STREAM_TYPE_OPUS,
    STREAM_TYPE_AMR,
    STREAM_TYPE_AMRWB,
} wr_stream_type_t;

typedef struct zh_hans_stream {
    audio_stream_type_t type;
    int block_size;
    bool is_open;
    int speed;
    int  length;
    char *buffer;
    wr_stream_type_t w_type;
} zh_hans_stream_t;


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
    int bytes_per_frame, bytes_per_sec;
    int bits_per_zh_hansple=16,zh_hansple_rate=16000,channels=1;
    bytes_per_frame = bits_per_zh_hansple/8*channels;
	bytes_per_sec   = bytes_per_frame*zh_hansple_rate;
    char *file=heap_caps_malloc(bufferlength+50, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    char *b;
    b=write_data2ram("RIFF",file,4);
    unsigned int filesize=bufferlength + 4 + 8 + 16 + 8;
    b=write_data2ram(&filesize,b,4);
    b=write_data2ram("WAVE",b,4);
    b=write_data2ram("fmt ",b,4);
    unsigned int fmtlength = 16;
    b=write_data2ram(&fmtlength,b,4);
    unsigned short int format=1; 
    b=write_data2ram(&format,b,2);      // Format
    b=write_data2ram(&channels,b,2);    // Channels
    b=write_data2ram(&zh_hansple_rate,b,4);// Samplerate
    b=write_data2ram(&bytes_per_sec,b,4);// Bytes per sec
    b=write_data2ram(&bytes_per_frame,b,2);// Bytes per frame
    b=write_data2ram(&bits_per_zh_hansple,b,2);// Bits per zh_hansple
    b=write_data2ram("data",b,4);
    b=write_data2ram(&bufferlength,b,4);
    for(int i=0;i<bufferlength;i++)
    {
        *b=buffer[i];
        b++;
    }
    heap_caps_free(buffer);
    *l=(b-file);
    return file;
}

static void zh_hans_translate_init()
{
    if(tts_handle==(esp_tts_handle_t *)0)
    {
        const esp_partition_t* part=esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "voice_data");
        if (part==0) { 
            ESP_LOGE(TAG, "machine_hw_speed_make_new.Couldn't find voice data partition!\n");
            return;
        }
        spi_flash_mmap_handle_t mmap;
        uint16_t* voicedata;
        esp_err_t err=esp_partition_mmap(part, 0, 3*1024*1024, SPI_FLASH_MMAP_DATA, (const void**)&voicedata, &mmap);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "machine_hw_speed_make_new.Couldn't map voice data partition!\n");
        }
        esp_tts_voice_t *voice=esp_tts_voice_set_init(&esp_tts_voice_template, voicedata); 
        tts_handle=esp_tts_create(voice);
    }
}
int zh_hans_set_speed(audio_element_handle_t self,int speed)
{
    zh_hans_stream_t *zh_hans = (zh_hans_stream_t *)audio_element_getdata(self);
    zh_hans->speed=speed;
    return 0;
}

static int zh_hans_translate(audio_element_handle_t self,char *text,char *buffer)
{
    zh_hans_stream_t *zh_hans = (zh_hans_stream_t *)audio_element_getdata(self);
    char *p=buffer,*sp;
    if(tts_handle==(esp_tts_handle_t *)0)
    {
        printf("zh_hans_translate init fail!\n");
        return 0;
    }
R:
    if (esp_tts_parse_chinese(tts_handle, text)) {
            int len[1]={0};
            do {
                short *pcm_data=esp_tts_stream_play(tts_handle, len,zh_hans->speed);   
                sp=(char *)pcm_data;
                len[0]=len[0]*2;
                for(int i=0;i<len[0];i++)
                {
                    *p=*sp;
                    p++;
                    sp++;
                    if((p-buffer)>=ZH_HANS_USER_BUF_SIZE)return (p-buffer);
                }
            } while(len[0]>0);
    }
    if((p-buffer)==0)
    {
        esp_tts_destroy(tts_handle);
        tts_handle=0;
        zh_hans_translate_init();
        goto R;
    }
    esp_tts_stream_reset(tts_handle);
    return (p-buffer);
}
static esp_err_t _zh_hans_open(audio_element_handle_t self)
{
    zh_hans_stream_t *zh_hans = (zh_hans_stream_t *)audio_element_getdata(self);

    audio_element_info_t info;
    char *uri = audio_element_get_uri(self);
    if (uri == NULL) {
        ESP_LOGE(TAG, "Error, uri is not set");
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "_zh_hans_open, uri:%s", uri);
//    char *path = strstr(uri, "/sdcard");
    char *path = strstr(uri, "/")+1;
    audio_element_getinfo(self, &info);
    if (path == NULL) {
        ESP_LOGE(TAG, "Error, need file path to open");
        return ESP_FAIL;
    }
    if (zh_hans->is_open) {
        ESP_LOGE(TAG, "already opened");
        return ESP_FAIL;
    }
    if (zh_hans->type == AUDIO_STREAM_READER) {
        zh_hans_translate_init();
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
        zh_hans->buffer=heap_caps_malloc(ZH_HANS_USER_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        info.total_bytes=zh_hans_translate(self,context,zh_hans->buffer);
        zh_hans->buffer=write_wav2ram(zh_hans->buffer,info.total_bytes,&info.total_bytes);
        printf("info.total_bytes=%d,ZH_HANS_USER_BUF_SIZE=%d\n", (int)info.total_bytes,ZH_HANS_USER_BUF_SIZE);
        zh_hans->length=ZH_HANS_USER_BUF_SIZE;
        ESP_LOGI(TAG, "File size is %d byte,pos:%d", (int)info.total_bytes, (int)info.byte_pos);
        zh_hans->is_open = true;
        if(zh_hans->speed>5)zh_hans->speed=5;
        if(zh_hans->speed<0)zh_hans->speed=0;
    } 
    return audio_element_setinfo(self, &info);
}

static int _zh_hans_read(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    zh_hans_stream_t *zh_hans = (zh_hans_stream_t *)audio_element_getdata(self);
    audio_element_info_t info;
    audio_element_getinfo(self, &info);
    
    int rlen;
    if(len<(info.total_bytes-info.byte_pos))rlen=len;
    else rlen=info.total_bytes-info.byte_pos;
    if(info.total_bytes<info.byte_pos)rlen=0;
    for(int i=0;i<rlen;i++)buffer[i]=zh_hans->buffer[i+info.byte_pos];
    if (rlen <= 0) {
        ESP_LOGW(TAG, "No more data,ret:%d", rlen);
        rlen = 0;
    } else {
        info.byte_pos += rlen;
        audio_element_setinfo(self, &info);
    }
    return rlen;
}

static int _zh_hans_process(audio_element_handle_t self, char *in_buffer, int in_len)
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

static esp_err_t _zh_hans_close(audio_element_handle_t self)
{
    zh_hans_stream_t *zh_hans = (zh_hans_stream_t *)audio_element_getdata(self);

    if (zh_hans->is_open) {
        heap_caps_free(zh_hans->buffer);
        zh_hans->is_open = false;
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

static esp_err_t _zh_hans_destroy(audio_element_handle_t self)
{
    zh_hans_stream_t *zh_hans = (zh_hans_stream_t *)audio_element_getdata(self);
    audio_free(zh_hans);
    return ESP_OK;
}

audio_element_handle_t zh_hans_stream_init(zh_hans_stream_cfg_t *config)
{
    audio_element_handle_t el;
    zh_hans_stream_t *zh_hans = audio_calloc(1, sizeof(zh_hans_stream_t));

    AUDIO_MEM_CHECK(TAG, zh_hans, return NULL);

    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.open = _zh_hans_open;
    cfg.close = _zh_hans_close;
    cfg.process = _zh_hans_process;
    cfg.destroy = _zh_hans_destroy;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.buffer_len = config->buf_sz;
    if (cfg.buffer_len == 0) {
        cfg.buffer_len = ZH_HANS_USER_BUF_SIZE;
    }

    cfg.tag = "hans";
    zh_hans->type = config->type;
    cfg.read = _zh_hans_read;
    el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, goto _zh_hans_init_exit);
    audio_element_setdata(el, zh_hans);
    return el;
_zh_hans_init_exit:
    audio_free(zh_hans);
    return NULL;
}
