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

#include "driver/uart.h"
#include "soc/uart_periph.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "esp_err.h"
#include "esp_log.h"
#include "uart1.h"

#define CMD_BUFSIZE      24
uint8_t rx_buf[CMD_BUFSIZE] = {0};

void parseCmd(char * cmd);

void parseGcode(char * cmd)
{
    char * tmp;
    char * str;
    //char g_code_cmd;

    str = strtok_r(cmd, " ", &tmp);
    //g_code_cmd = str[0];
    
    int cmd_id = atoi(str);

    cmd_id = cmd_id & 0x00ff;
    
    if( cmd_id == 0x11)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0x12)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0x13)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0x14)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0x80)
    {
        printf("cmd_id = %d\n",cmd_id);   
    }
    else if (cmd_id == 0x81)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0xff)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else 
    {
        printf("num_id id error\n");
    }

    //printf("tmp %s\n",tmp);
    while(tmp!=NULL)
    {
        str = strtok_r(0, " ", &tmp);

        if((str[0]=='M') || (str[0]=='m'))
        {
            cmd_id = atoi(str+1);
            printf("cmd_id = M%d\n",cmd_id);

        }
        else if((str[0]=='E') || (str[0]=='e'))
        {
            cmd_id = atoi(str+1);
            printf("cmd_id = E%d\n",cmd_id);

        }else if((str[0]=='P') || (str[0]=='p'))
        {
            cmd_id = atoi(str+1);
            printf("cmd_id = P%d\n",cmd_id);

        }
        else if((str[0]=='D') || (str[0]=='d'))
        {
            cmd_id = atoi(str+1);
            printf("cmd_id = D%d\n",cmd_id);

        }
        else if((str[0]=='M') || (str[0]=='m'))
        {

        }
        
        
    }

}

void parseCmd(char * cmd)
{
    if((cmd[0]=='g') || (cmd[0]=='G'))
    { 
        parseGcode(cmd+1);
    }
}


void uart_ringbuf_init(void)
{
    drv_ringbuf_init((RING_BUF_DEF_STRUCT*)&s_tx_ring_buf, s_link_tx_buf, DATA_TX_BUFSIZE);
    drv_ringbuf_init((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, s_link_rx_buf, DATA_RX_BUFSIZE);
}


#define ECHO_TEST_TXD (46)
#define ECHO_TEST_RXD (48)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (UART_NUM_1)
#define ECHO_UART_BAUD_RATE     (115200)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)
#define PACKET_READ_TICS        (100 / portTICK_RATE_MS)
#define BUF_SIZE (256)
#define ECHO_READ_TOUT          (3) // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks


void uart1_init(void)
{
    uart_ringbuf_init();
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    ESP_ERROR_CHECK(uart_set_mode(UART_NUM_1, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_NUM_1, ECHO_READ_TOUT));

}

int uart1_read_data(void *read_data)
{
    int len = uart_read_bytes(UART_NUM_1, read_data, BUF_SIZE, PACKET_READ_TICS);
    drv_ringbuf_write((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, read_data, len);
    
    drv_ringbuf_read((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, CMD_BUFSIZE, rx_buf);

    //printf("len = %d\n",sizeof(rx_buf));
    printf("rx_buf = %s\r\n",rx_buf);
    parseCmd(&rx_buf);
    printf("rx_buf1 = %s\r\n",rx_buf);

    drv_ringbuf_flush((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf);
    ringbuff_rx_reset((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf,CMD_BUFSIZE);
    return len;
}

void uart1_send_data(void *send_data,size_t length)
{
    uart_write_bytes(UART_NUM_1, send_data, length);
    
}