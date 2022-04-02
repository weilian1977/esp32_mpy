
#ifndef __SPEECH_CN_H__
#define __SPEECH_CN_H__

#include "audio_element.h"
#include "audio_pipeline.h"
#include "esp_afe_sr_iface.h"
//esp_afe_sr_iface_t *afe_handle = NULL;

audio_pipeline_handle_t pipeline_rec;
audio_element_handle_t i2s_stream_reader;
//void setup_player(void);

void speech_cn_init(void);
void voice_read_task();
void voice_read_task_c();
#endif
