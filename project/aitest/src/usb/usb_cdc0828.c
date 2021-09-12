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
#define debug(fmt, ...)   if(0) printf(fmt, ##__VA_ARGS__)

extern void usbdbg_data_in(void *buffer, int length);
extern void usbdbg_data_out(void *buffer, int length);
extern void usbdbg_control(void *buffer, uint8_t brequest, uint32_t wlength);

uint32_t usb_cdc_buf_len()
{
    return 1024;
}

uint32_t usb_cdc_get_buf(uint8_t *buf, uint32_t len)
{
    return 0;
}

#include "py/runtime.h"
#include "py/mphal.h"
void cdc_task_serial_mode(void)
{
    if (tud_ready() && userial_open) {
        // connected and there are data available
        if (tud_cdc_available()) {
            uint8_t usb_rx_buf[CONFIG_USB_CDC_RX_BUFSIZE];
            uint32_t len = tud_cdc_read(usb_rx_buf, CONFIG_USB_CDC_RX_BUFSIZE);
            for (int i = 0; i < len; i++) {
                if (usb_rx_buf[i] == mp_interrupt_char) {
                    debug("keyboard_interrupt. ");mp_sched_keyboard_interrupt();
                } else {
                    ringbuf_put(&stdin_ringbuf, usb_rx_buf[i]);
                }
                debug("%c", usb_rx_buf[i]);
            }
            debug(" avail %d, put %d bytes\n", ringbuf_avail(&stdin_ringbuf), len);
        }
    } else {
      vTaskDelay(pdMS_TO_TICKS(5));
    }
}

#define CDC_WRDAT(w_buf, w_len) do {  \
  int pos = 0, left = w_len;  \
  while(left > 0) { \
    pos += tud_cdc_write(&w_buf[pos], left); \
    left = w_len-pos; \
    tud_cdc_write_flush();  \
  } \
} while(0)
void usb_tx_strn(const char *str, size_t len) {
    debug("usb_tx_strn %d bytes\n", len);
    if(dbg_mode_enabled == false)
      CDC_WRDAT(str, len);
}

void cdc_task_debug_mode(void)
{
    if (tud_cdc_connected()) {
      if(tud_cdc_available()) {
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
    } else {
      vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    if (p_line_coding->bit_rate == IDE_BAUDRATE_SLOW ||
            p_line_coding->bit_rate == IDE_BAUDRATE_FAST) {
        dbg_mode_enabled = 1;
    } else {
        dbg_mode_enabled = 0;
    }

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

void cdc_loop() {
    while(true) {
        if(is_dbg_mode_enabled()) {
            cdc_task_debug_mode();
        } else {
            cdc_task_serial_mode();
        }
    }
}

#if CONFIG_IDF_TARGET_ESP32S3
static void usb_otg_router_to_internal_phy()
{
  uint32_t *usb_phy_sel_reg = (uint32_t *)(0x60008000 + 0x120);
  *usb_phy_sel_reg |= BIT(19) | BIT(20);
}
#endif

int usb_cdc_init(void)
{
    vTaskDelay(100 / portTICK_PERIOD_MS);
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        
        tinyusb_config_t tusb_cfg = {
            .descriptor = NULL,
            .string_descriptor = NULL,
            .external_phy = false // In the most cases you need to use a `false` value
        };
#if CONFIG_IDF_TARGET_ESP32S3
        usb_otg_router_to_internal_phy();
#endif
        ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
        xTaskCreatePinnedToCore(cdc_loop, "cdc_loop", USB_CDC_TASK_STACK_SIZE / sizeof(StackType_t), NULL, USB_CDC_TASK_PRIORITY, NULL, 1);
    }
    return 0;
}

void cdc_printf(const char *fmt, ...)
{  
  if(!dbg_mode_enabled && tud_ready() && userial_open) {  
    va_list ap;
    char p_buf[256]; 
    int p_len;
   
    va_start(ap, fmt);
    p_len = vsprintf(p_buf, fmt, ap);
    va_end(ap);//debug("cdc_printf size %d, cdc open %d\n", p_len, tud_cdc_connected());
    CDC_WRDAT(p_buf, p_len);    
  }
}


#endif // CONFIG_USB_ENABLED
