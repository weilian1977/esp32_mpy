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

//#include "tusb.h"
//#include "bsp/board.h"
#include "tusb.h"
#include "freertos/FreeRTOS.h"
#include "esp_task.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
#define SUB_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 1)
#define SUB_TASK_STACK_SIZE      (16 * 1024)

void cdc_task(void);
void sub_task(void *pvParameter);

#if CONFIG_IDF_TARGET_ESP32S3
static void usb_otg_router_to_internal_phy()
{
  uint32_t *usb_phy_sel_reg = (uint32_t *)(0x60008000 + 0x120);
  *usb_phy_sel_reg |= BIT(19) | BIT(20);
}
#endif

/*------------- MAIN -------------*/
int main(void)
{
#if CONFIG_IDF_TARGET_ESP32S3
  usb_otg_router_to_internal_phy();
#endif
  tusb_init();
  printf("tusb_init\n");
  xTaskCreatePinnedToCore(sub_task, "sub_task", SUB_TASK_STACK_SIZE / sizeof(StackType_t), NULL, SUB_TASK_PRIORITY, NULL, 0);
  while (1)
  {
    tud_task(); // tinyusb device task
  }

  return 0;
}

void app_main(void)
{
  main();
}

void sub_task(void *pvParameter)
{
    while(1)
    {
      cdc_task();
    }
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void cdc_task(void)
{
  // connected() check for DTR bit
  // Most but not all terminal client set this when making connection
  if ( tud_cdc_connected() )
  {
    // connected and there are data available
    if ( tud_cdc_available() )
    {
      // read datas
      char buf[64];
      uint32_t count = tud_cdc_read(buf, sizeof(buf));
      (void) count;
      // Echo back
      // Note: Skip echo by commenting out write() and write_flush()
      // for throughput test e.g
      //    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
      tud_cdc_write(buf, count);
      tud_cdc_write_flush();
    }
  }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
  (void) itf;
  (void) rts;

  // TODO set some indicator
  if ( dtr )
  {
    // Terminal connected
  }else
  {
    // Terminal disconnected
  }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
  (void) itf;
}

