
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mphalport.h"
#include "drv_ble.h"

//#include "message.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "esp_err.h"
#include "esp_log.h"


static const char *tag = "drv_ble";

#define BLE_BUFF_SIZE (1024)
static uint8_t ble_ringbuf_array[BLE_BUFF_SIZE];

uint8_t long_msg_send_flag = false;  //for transmit file
uint8_t long_msg_recive_flag = false;  //for recive file

ringbuf_t ble_ringbuf = {ble_ringbuf_array, sizeof(ble_ringbuf_array), 0, 0};

ble_status_t ble_status;

ble_irq_func ble_irq = NULL;
static uint32_t ble_type_event;

void ble_ringbuf_init(void)
{
    ble_ringbuf.size = BLE_BUFF_SIZE;
    ble_ringbuf.iget = 0;
    ble_ringbuf.iput = 0;        
    return;
}

void ble_init(void)
{
    ble_status.protocol = MSG_PROTOCOL_V2;
    ble_status.state = BLE_STATE_DISCONNECT;
    ble_status.heart_enable = true;
    ble_ringbuf_init();
    return;
}

void ble_protocol_v1_msg_rx(const uint8_t *pdata, uint32_t length)
{
    uint32_t i;
    if(pdata[0] == 0x84)
    {
        ble_ringbuf_init();
        return;
    }
    if(ringbuf_free(&ble_ringbuf) >= length)
    {
        for(i = 0; i < length; i++)
        {
            ringbuf_put(&ble_ringbuf, pdata[i]);
        }
    }
    else
    {
        ble_ringbuf_init();
    }
}





/*
void ble_protocol_v2_msg_rx(const uint8_t *pdata, uint32_t length)
{
    
    uint8_t i;
    uint8_t translation_flag;
    if((pdata[0] == MSG_FRAMER_HEADER) && (pdata[1] == 0x03) 
        && (pdata[2] == CMD_CANCEL) && (pdata[3] == 0x99) && (pdata[4] == 0x50))
    {
        ble_ringbuf_init();
        //message_move_speed_both(0, 0);
        long_msg_recive_flag = false;
        ///message_normal_response(MESSAGE_RX_OK);
        return;
    }
        
    translation_flag = false;
    if(ringbuf_free(&ble_ringbuf) >= length)
    {
        for(i = 0; i < length; i++)
        {
            if((pdata[i] == MSG_FRAMER_TRANSLATION) && (long_msg_recive_flag == false))
            {
                translation_flag = true;
            }
            else
            {
                ringbuf_put(&ble_ringbuf, pdata[i] + translation_flag * CHAR_TRANSLATION_OFFSET);
                translation_flag = false;
            }
        }

    }
    else
    {
        ble_ringbuf_init();
    }
}
*/
uint32_t ble_get_msg_len(void)
{
    return ringbuf_avail(&ble_ringbuf);
}

uint32_t ble_get_msg(uint8_t *buf)
{
    uint32_t i;
    uint32_t len;
    len = ringbuf_avail(&ble_ringbuf);
    
    //ESP_LOGE(tag, "ringbuf_avail:%d", len);
    if(len)
    {
        for(i = 0; i < len; i++)
        {
            buf[i] = ringbuf_get(&ble_ringbuf);
        }
    }
    
    return len;
}

void ble_irq_event(int32_t event)
{
    ble_type_event = event;
    
#if 0
    if(ble_irq)
    {
        ble_irq(event);
    }
#endif

}

uint32_t ble_get_event(void)
{
    uint32_t event;
    /* read clear*/
    event = ble_type_event;
    ble_type_event = 0;
    return event;
}

void ble_irq_func_register(ble_irq_func func)
{
    ble_irq = func;
    return;
}

void ble_set_connect_state(uint32_t connect_state)
{
    ESP_LOGI(tag, "ble_set_connect_state");
    ble_status.state = connect_state;
}

uint8_t ble_get_connect_state(void)
{
    return ble_status.state;
}

void ble_set_protocol_version(uint8_t version)
{
    ble_status.protocol = version;
}

uint8_t ble_get_protocol_version(void)
{
    return ble_status.protocol;
}

void ble_set_heart_enable(uint8_t heart_enable)
{
    ble_status.heart_enable = heart_enable;
}

uint8_t ble_get_heart_enable(void)
{
    return ble_status.heart_enable;
}

void ble_recive_file(uint8_t *buffer, uint32_t len)
{
    
    int err = 0;
    const char *path_out = NULL;
    char *path = "/main.py";
    unsigned int wlen = 0;
    FIL fp;
    mp_vfs_mount_t *vfs_mount = mp_vfs_lookup_path(path, &path_out);
    err = f_open(&(((fs_user_mount_t *)vfs_mount->obj)->fatfs), &fp, path_out, FA_WRITE | FA_CREATE_ALWAYS);
    if(err)
    {
        ESP_LOGE(tag, "open file fail.err:%d", err);
    }
    f_write(&fp, buffer, len, &wlen);
    f_sync(&fp);
    f_close(&fp);

}

int32_t ble_rx_data(uint8_t *buffer, uint32_t len)
{
    uint8_t i;
#if 0
    if(ble_status.protocol == MSG_PROTOCOL_V1)
    {
        ble_protocol_v1_msg_rx(buffer, len);

    }
    if(ble_status.protocol == MSG_PROTOCOL_V2)
    {
        ble_protocol_v2_msg_rx(buffer, len);
    }
#endif
    if(ringbuf_free(&ble_ringbuf) >= len)
    {
        for(i = 0; i < len; i++)
        {
            ringbuf_put(&ble_ringbuf, buffer[i]);
        }
    }
    else
    {
        ble_ringbuf_init();
    }

    ble_irq_event(IRQ_TYPE_RECIVE);

    return ESP_OK;
}

int32_t ble_tx_data(uint8_t *buffer, uint32_t len)
{
    return ESP_OK;
}

int32_t ble_msg_process(uint8_t *buffer, uint32_t len)
{
    return ESP_OK;
}

