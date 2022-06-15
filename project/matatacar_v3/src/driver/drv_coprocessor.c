#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "drv_coprocessor.h"

static const char *TAG = "DRV_COPROCESSOR";

#define RX_BUF_SIZE    128
#define TXD_PIN (GPIO_NUM_43)
#define RXD_PIN (GPIO_NUM_44)


sensor_data_type sensor_value = {0};

uint8_t frame_buffer[RX_BUF_SIZE];

void drv_coprpcessor_init(void)
{
    const uart_config_t uart_config =
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    sensor_value.is_auto_color = true;
    sensor_value.ir_threshold = 400;
}

void drv_coprpcessor_printf(char *fmt,...)
{
    va_list ap;
    char string[64]; 
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    uart_write_bytes(UART_NUM_0, (uint8_t*)string, strlen(string));
    va_end(ap);
}

uint16_t read_uint16_t_data(int idx)
{
    val2byte.byteVal[0] = frame_buffer[2 + idx];
    val2byte.byteVal[1] = frame_buffer[3 + idx];
    return val2byte.shortVal;
}

void parse_command(void)
{
    sensor_value.left_ir = read_uint16_t_data(0);
    sensor_value.right_ir = read_uint16_t_data(2);
    sensor_value.color_sensor_off = read_uint16_t_data(4);
    sensor_value.r_value = read_uint16_t_data(6);
    sensor_value.g_value = read_uint16_t_data(8);
    sensor_value.b_value = read_uint16_t_data(10);
}

void drv_coprpcessor_task(void *arg)
{
    static bool is_frame_start = false;
    static uint8_t prevdata = 0;
    static uint8_t frame_index = 0;
    uint8_t *data = (uint8_t*) malloc(RX_BUF_SIZE + 1);
    unsigned int  size = 0;
    // drv_coprpcessor_init();
    while (1)
    {
        uart_get_buffered_data_len(UART_NUM_0, &size);
        if(size > 0)
        {
            const int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, 5 / portTICK_RATE_MS);
            if (rxBytes > 0) 
            {
                for(uint8_t i = 0; i < rxBytes; i++)
                {
                    if((data[i] == 0xfe) && (is_frame_start == false))
                    {
                        if(prevdata == 0xff)
                        {
                            frame_buffer[frame_index++] = 0xff;
                            frame_buffer[frame_index++] = 0xfe;
                            is_frame_start = true;
                        }
                    }
                    else
                    {
                        prevdata = data[i];
                        if(is_frame_start)
                        {
                            frame_buffer[frame_index++] = data[i];
                            if(frame_index > 64)
                            {
                                frame_index = 0;
                                prevdata = 0;
                                is_frame_start = false;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
        if(frame_index >= 14)
        {
            parse_command();
            frame_index = 0;
            prevdata = 0;
            is_frame_start = false;
        }
    }
}