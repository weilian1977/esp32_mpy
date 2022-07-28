/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_task.h"
#include "soc/cpu.h"
#include "esp_log.h"
#include "esp_wifi.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/spiram.h"
#endif

#include "py/stackctrl.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/persistentcode.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "shared/readline/readline.h"
#include "shared/runtime/pyexec.h"
#include "uart.h"
#include "usb.h"
//#include "usb_msc.h"
#include "usb_serial_jtag.h"
#include "modmachine.h"
#include "modnetwork.h"
#include "mpthreadport.h"
#include "speech_cn.h"
#include "system_management.h"
#include "power_management.h"
#include "usb_detect.h"
#include "drv_aw20144.h"
#include "drv_infrared_transceiver.h"
#include "drv_coprocessor.h"
#include "audio_player.h"
#include "audio_record.h"

#if MICROPY_BLUETOOTH_NIMBLE
#include "extmod/modbluetooth.h"
#endif

#include "driver_update.h"
#include "mt_event_mechanism.h"
#include "mt_stop_python_thread.h"
#include "drv_step_motor.h"
#include "i2s_mic.h"

// MicroPython runs as a task under FreeRTOS
#define MP_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 1)
#define MP_TASK_STACK_SIZE      (16 * 1024)

// Set the margin for detecting stack overflow, depending on the CPU architecture.
#if CONFIG_IDF_TARGET_ESP32C3
#define MP_TASK_STACK_LIMIT_MARGIN (2048)
#else
#define MP_TASK_STACK_LIMIT_MARGIN (1024)
#endif

bool first_start_flag = true;
extern void ble_prph_main(void);

void repl_push_chars(const uint8_t *data, int32_t len);

int vprintf_null(const char *format, va_list ap) {
    // do nothing: this is used as a log target during raw repl mode
    return 0;
}

void mp_task(void *pvParameter) {
    volatile uint32_t sp = (uint32_t)get_sp();
    #if MICROPY_PY_THREAD
    mp_thread_init(pxTaskGetStackStart(NULL), MP_TASK_STACK_SIZE / sizeof(uintptr_t));
    #endif
    #if CONFIG_USB_ENABLED
    // usb cdc&msc init
    usb_init();
    uart_stdout_init();
    #elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    usb_serial_jtag_init();
    #else
    uart_stdout_init();
    #endif
    machine_init();

    size_t mp_task_heap_size;
    void *mp_task_heap = NULL;

    #if CONFIG_SPIRAM_USE_MALLOC
    // SPIRAM is issued using MALLOC, fallback to normal allocation rules
    mp_task_heap = NULL;
    #elif CONFIG_ESP32_SPIRAM_SUPPORT
    // Try to use the entire external SPIRAM directly for the heap
    mp_task_heap = (void *)SOC_EXTRAM_DATA_LOW;
    switch (esp_spiram_get_chip_size()) {
        case ESP_SPIRAM_SIZE_16MBITS:
            mp_task_heap_size = 2 * 1024 * 1024;
            break;
        case ESP_SPIRAM_SIZE_32MBITS:
        case ESP_SPIRAM_SIZE_64MBITS:
            mp_task_heap_size = 4 * 1024 * 1024;
            break;
        default:
            // No SPIRAM, fallback to normal allocation
            mp_task_heap = NULL;
            break;
    }
    #elif CONFIG_ESP32S2_SPIRAM_SUPPORT || CONFIG_ESP32S3_SPIRAM_SUPPORT
    // Try to use the entire external SPIRAM directly for the heap
    size_t esp_spiram_size = esp_spiram_get_size();
    if (esp_spiram_size > 0) {
        // mp_task_heap = (void *)SOC_EXTRAM_DATA_HIGH - esp_spiram_size;
        // mp_task_heap_size = esp_spiram_size;
        mp_task_heap_size = 4 * 1024 * 1024;
        mp_task_heap = heap_caps_malloc(mp_task_heap_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if(mp_task_heap == NULL)
        {
            printf("malloc heap for mp task failed, heap size if %d\n", mp_task_heap_size);
        }
        printf("esp_spiram_size:%d, heap size:%d\n", esp_spiram_size, mp_task_heap_size);
    }
    #endif

    if (mp_task_heap == NULL) {
        // Allocate the uPy heap using malloc and get the largest available region,
        // limiting to 1/2 total available memory to leave memory for the OS.
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0)
        size_t heap_total = heap_caps_get_total_size(MALLOC_CAP_8BIT);
        #else
        multi_heap_info_t info;
        heap_caps_get_info(&info, MALLOC_CAP_8BIT);
        size_t heap_total = info.total_free_bytes + info.total_allocated_bytes;
        #endif
        mp_task_heap_size = MIN(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT), heap_total / 2);
        mp_task_heap = malloc(mp_task_heap_size);
    }
//soft_reset:
    // initialise the stack pointer for the main thread
    mp_stack_set_top((void *)sp);
    mp_stack_set_limit(MP_TASK_STACK_SIZE - MP_TASK_STACK_LIMIT_MARGIN);
    gc_init(mp_task_heap, mp_task_heap + mp_task_heap_size);

    for (;;)
    {
        mp_init();
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
        readline_init0();

        // initialise peripherals
        machine_pins_init();
        #if MICROPY_PY_MACHINE_I2S
        machine_i2s_init0();
        #endif

        mt_thread_table_init_t();
        int8_t thread_id = -1;
        mt_thread_add_thread_to_table_t(NULL, &thread_id);

        // run boot-up scripts
        pyexec_frozen_module("_boot.py");
        pyexec_file_if_exists("boot.py");
        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL)
        {        
            if(first_start_flag)
            {
                pyexec_file_if_exists("power_on.py");
            }
            int ret = pyexec_file_if_exists("system_call.py");
            if (ret & PYEXEC_FORCED_EXIT)
            {
                goto soft_reset_exit;
            }
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
        if(first_start_flag)
        {
            first_start_flag = false;
        }
        for (;;)
        {
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL)
            {
                vprintf_like_t vprintf_log = esp_log_set_vprintf(vprintf_null);
                if (pyexec_raw_repl() != 0)
                {
                    break;
                }
                esp_log_set_vprintf(vprintf_log);
            }
            else
            {
                if (pyexec_friendly_repl() != 0)
                {
                    break;
                }
            }
        }

        soft_reset_exit:

        play_stop();
        recorder_stop();
        motor_stop(MOTOR_LEFT);
        motor_stop(MOTOR_RIGHT);
        #if MICROPY_BLUETOOTH_NIMBLE
        mp_bluetooth_deinit();
        #endif

        machine_timer_deinit_all();

        #if MICROPY_PY_THREAD
        mp_thread_deinit();
        #endif

        mt_thread_table_deinit_t();
        gc_sweep_all();

        mp_hal_stdout_tx_str("MPY: soft reboot\r\n");

        // deinitialise peripherals
        machine_pwm_deinit_all();
        // TODO: machine_rmt_deinit_all();
        machine_pins_deinit();
        machine_deinit();
        usocket_events_deinit();

        mp_deinit();
        fflush(stdout);
        // goto soft_reset;
    }
}

void boardctrl_startup(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
}

void app_main(void) {
    // Hook for a board to run code at start up.
    // This defaults to initialising NVS.
    MICROPY_BOARD_STARTUP();
    mt_eve_init_t();
    driver_update_task_init();
    esp_wifi_stop();
    ble_prph_main();
    xTaskCreatePinnedToCore(drv_coprpcessor_task, "drv_coprpcessor_task", DRV_COPROCESSOR_TASK_STACK_SIZE / sizeof(StackType_t), NULL, DRV_COPROCESSOR_TASK_PRIORITY, NULL, 0);
    xTaskCreatePinnedToCore(step_motor_task, "step_motor_task", STEP_MOTOR_TASK_STACK_SIZE / sizeof(StackType_t), NULL, 0, NULL, 0);
    
    audio_play_init();
    audio_recorder_init();
    //speech_cn_init();
    digital_mic_init();
    xTaskCreatePinnedToCore(system_management_task, "system_management_task", SYSTEM_MANAGEMENT_TASK_STACK_SIZE / sizeof(StackType_t), NULL, SYSTEM_MANAGEMENT_TASK_PRIORITY, NULL, 0);
    xTaskCreatePinnedToCore(mp_task, "mp_task", MP_TASK_STACK_SIZE / sizeof(StackType_t), NULL, MP_TASK_PRIORITY, &mp_main_task_handle, MP_TASK_COREID);
    xTaskCreatePinnedToCore(driver_ir_task, "driver_ir_task", INFRARED_TRANSCEIVER_TASK_STACK_SIZE / sizeof(StackType_t), NULL, INFRARED_TRANSCEIVER_TASK_PRIORITY, NULL, 0);
}

void nlr_jump_fail(void *val) {
    printf("NLR jump failed, val=%p\n", val);
    usb_status_update();
    if(is_usb_detected() == true)
    {
        printf("usb detected, power off!\n");
        power_off();
        while(1);
    }
    else
    {
        esp_restart();
    }
}

void mp_task_reset(void)
{
    // uint8_t cmd = CHAR_CTRL_D;
    // repl_push_chars(&cmd, 1);
    esp_restart();
}

// modussl_mbedtls uses this function but it's not enabled in ESP IDF
void mbedtls_debug_set_threshold(int threshold) {
    (void)threshold;
}

void *esp_native_code_commit(void *buf, size_t len, void *reloc) {
    len = (len + 3) & ~3;
    uint32_t *p = heap_caps_malloc(len, MALLOC_CAP_EXEC);
    if (p == NULL) {
        m_malloc_fail(len);
    }
    if (reloc) {
        mp_native_relocate(reloc, buf, (uintptr_t)p);
    }
    memcpy(p, buf, len);
    return p;
}

/* call this funtionc only when repl is running  */
void repl_push_chars(const uint8_t *data, int32_t len)
{
  uint8_t c = 0;
  for(int32_t i = 0; i < len; i++)
  {
    c = data[i];
    
    if(c == mp_interrupt_char) 
    {
      // inline version of mp_keyboard_interrupt();
      MP_STATE_MAIN_THREAD(mp_pending_exception) = MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception));
#if MICROPY_ENABLE_SCHEDULER
      if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) 
      {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
      }
#endif /* MICROPY_ENABLE_SCHEDULER */
    }
    else 
    {
      // this is an inline function so will be in IRAM
      ringbuf_put(&stdin_ringbuf, c);
    }
  }
}