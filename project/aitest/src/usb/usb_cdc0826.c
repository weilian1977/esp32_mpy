/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "esp_task.h"
#include "py/mpthread.h"

#include "tusb.h"
#include "usb_cdc.h"
#include "tinyusb.h"
#include "esp_log.h"

#if CONFIG_USB_ENABLED
#define DBG_MAX_PACKET      (64)
#define IDE_BAUDRATE_SLOW   (921600)
#define IDE_BAUDRATE_FAST   (12000000)

// MicroPython runs as a task under FreeRTOS
#define USB_CDC_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 2)
#define USB_CDC_TASK_STACK_SIZE      (16 * 1024)

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

volatile uint8_t  dbg_mode_enabled = 1;
static bool linecode_set = false, userial_open = false;
#define debug(fmt, ...)   if(1) printf(fmt, ##__VA_ARGS__)

extern void usbdbg_data_in(void *buffer, int length);
extern void usbdbg_data_out(void *buffer, int length);
extern void usbdbg_control(void *buffer, uint8_t brequest, uint32_t wlength);

uint8_t rx_ringbuf_array[1024];
uint8_t tx_ringbuf_array[1024];
volatile ringbuf_t rx_ringbuf;
volatile ringbuf_t tx_ringbuf;

static bool cdc_rx_any(void) {
    return rx_ringbuf.iput != rx_ringbuf.iget;
}

static int cdc_rx_char(void) {
    return ringbuf_get((ringbuf_t*)&rx_ringbuf);
}

static bool cdc_tx_any(void) {
    return tx_ringbuf.iput != tx_ringbuf.iget;
}

static int cdc_tx_char(void) {
    return ringbuf_get((ringbuf_t*)&tx_ringbuf);
}

uint32_t usb_cdc_buf_len()
{
    return ringbuf_avail((ringbuf_t*)&tx_ringbuf);
}

uint32_t usb_cdc_get_buf(uint8_t *buf, uint32_t len)
{
    int i=0;
    for (; i<len; i++) {
        buf[i] = ringbuf_get((ringbuf_t*)&tx_ringbuf);
        if (buf[i] == -1) {
            break;
        }
    }
    return i;
}

#include "py/runtime.h"
#include "py/mphal.h"
void cdc_task_serial_mode(void)
{
    if (tud_cdc_connected() && (tud_cdc_available() || cdc_tx_any())) {printf("%s: %d\n", __func__, __LINE__);
        // connected and there are data available
        while (tud_cdc_available()) {
            uint8_t c;printf("%s: %d\n", __func__, __LINE__);
            uint32_t count = tud_cdc_read(&c, 1);printf("%s: %d\n", __func__, __LINE__);
            (void)count;
            ringbuf_put((ringbuf_t*)&rx_ringbuf, c);printf("%s: %d\n", __func__, __LINE__);
            if (c == mp_interrupt_char) {
                mp_sched_keyboard_interrupt();printf("%s: %d\n", __func__, __LINE__);
            } else {
                printf("%s: %d\n", __func__, __LINE__);ringbuf_put(&stdin_ringbuf, c);printf("%s: %d\n", __func__, __LINE__);
            }
            printf("cdc_task_serial_mode get char 0x%X\n", c);
        }

        int chars = 0;
        while (cdc_tx_any()) {
            if (chars < DBG_MAX_PACKET) {
               tud_cdc_write_char(cdc_tx_char());
               chars++;
            } else {
               chars = 0;
               tud_cdc_write_flush();
            }
        }
        tud_cdc_write_flush();
    }else {
      vTaskDelay(pdMS_TO_TICKS(1000));
      printf("no char received!\n");
    }
}

void usb_tx_strn(const char *str, size_t len) {
    for (int i=0;i<len;i++) {
      while(ringbuf_put(&tx_ringbuf, (uint8_t )str[i]) < 0) {
        printf("Error: usb_cdc tx_ringbuf overflow!\n");
        vTaskDelay(pdMS_TO_TICKS(5));
      }
    }
}

void cdc_task_debug_mode(void)
{
    if ( tud_cdc_connected() && tud_cdc_available() ) {debug("cdc_task_debug_mode %d\n", tud_cdc_available());
        uint8_t buf[DBG_MAX_PACKET];
        uint32_t count = tud_cdc_read(buf, 6);
        if (count < 6) {
            //Shouldn't happen
            return;
        }
        // assert buf[0] == '\x30';
        uint8_t request = buf[1];
        uint32_t xfer_length = *((uint32_t*)(buf + 2));
        usbdbg_control(buf + 6, request, xfer_length);

        while (xfer_length) {
            if (request & 0x80) {
                // Device-to-host data phase
                int bytes = MIN(xfer_length, DBG_MAX_PACKET);
                if (bytes <= tud_cdc_write_available()) {
                    xfer_length -= bytes;
                    usbdbg_data_in(buf, bytes);
                    tud_cdc_write(buf, bytes);
                }
            } else {
                // Host-to-device data phase
                int bytes = MIN(xfer_length, DBG_MAX_PACKET);
                uint32_t count = tud_cdc_read(buf, bytes);
                if (count == bytes) {
                    xfer_length -= count;
                    usbdbg_data_out(buf, count);
                }
            }
        }
        tud_cdc_write_flush();
    }
}

#include "soc/spinlock.h"

#define MAX_MUTEX_WAIT_TICKS ((10 + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS)
static SemaphoreHandle_t s_log_mutex = NULL;
void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    if (p_line_coding->bit_rate == IDE_BAUDRATE_SLOW ||
            p_line_coding->bit_rate == IDE_BAUDRATE_FAST) {
        dbg_mode_enabled = 1;
    } else {
        dbg_mode_enabled = 0;
    }
    tx_ringbuf.iget = 0;
    tx_ringbuf.iput = 0;

    rx_ringbuf.iget = 0;
    rx_ringbuf.iput = 0;
    linecode_set = true;
    debug("tud_cdc_line_coding_cb dbg_mode_enabled %d\n", dbg_mode_enabled);
}

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{  
  if(!dtr) {
    if(linecode_set && !userial_open)
      userial_open = true;
    else if(userial_open && !linecode_set) {
      userial_open = false;   
    }
    
    if(linecode_set)
      linecode_set = false;
      
  } else {
      userial_open = true;
  }

  debug("tud_cdc_line_state_cb userial_open %d, line_state %d\n", userial_open, (dtr|(rts<<1)));
}

bool is_dbg_mode_enabled(void)
{
    return dbg_mode_enabled;
}

static void cdc_loop(void *pvParameters) {
    (void)pvParameters;
    while(true) {
        if(is_dbg_mode_enabled()) {
            cdc_task_debug_mode();
        } else {
            printf("%s: %d\n", __func__, __LINE__);cdc_task_serial_mode();
        }
    }
}

int usb_cdc_init(void)
{
    static bool initialized = false;

    linecode_set = false;userial_open = false;
    if (!initialized) {
        initialized = true;

        rx_ringbuf.buf = rx_ringbuf_array;
        rx_ringbuf.size = sizeof(rx_ringbuf_array);
        rx_ringbuf.iget = 0;
        rx_ringbuf.iput = 0;

        tx_ringbuf.buf = tx_ringbuf_array;
        tx_ringbuf.size = sizeof(tx_ringbuf_array);
        tx_ringbuf.iget = 0;
        tx_ringbuf.iput = 0;
        //tusb_init();
        tinyusb_config_t tusb_cfg = {
            .descriptor = NULL,
            .string_descriptor = NULL,
            .external_phy = false // In the most cases you need to use a `false` value
        };

        ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
        xTaskCreatePinnedToCore(cdc_loop, "cdc_loop", USB_CDC_TASK_STACK_SIZE / sizeof(StackType_t), NULL, USB_CDC_TASK_PRIORITY, NULL, 1);
    }
    return 0;
}

void cdc_printf(const char *fmt, ...)
{  
  if (unlikely(!s_log_mutex)) {
        s_log_mutex = xSemaphoreCreateMutex();
  }
  if (likely(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)) {
    if(xSemaphoreTake(s_log_mutex, MAX_MUTEX_WAIT_TICKS)!= pdTRUE) {
      debug("Busy mutex\n");
      return;
    }
  }
  //debug("cdc_printf usb ready %d\n", tud_ready());  
  if(!dbg_mode_enabled && tud_ready() && userial_open) {  
    va_list ap;
    char p_buf[256]; 
    int p_len;

    //vTaskSuspendAll();    
    va_start(ap, fmt);
    p_len = vsprintf(p_buf, fmt, ap);
    va_end(ap);debug("cdc_printf size %d, cdc open %d\n", p_len, tud_cdc_connected());
    
    int pos = 0, left = p_len;
    while(left > 0) { \
      pos += tud_cdc_write(&p_buf[pos], left); \
      left = p_len-pos; \
    } \
    tud_cdc_write_flush();
    //xTaskResumeAll();
  }  
  if (likely(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)) {
    xSemaphoreGive(s_log_mutex);
  }
}


#endif // CONFIG_USB_ENABLED
