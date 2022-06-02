#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "drv_infrared_transceiver.h"
#include "driver/rmt.h"
#include "ir_tools.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "DRV_INFRARED_TRANSCEIVER";

static rmt_channel_t infrared_tx_channel = CONFIG_EXAMPLE_RMT_TX_CHANNEL;
static rmt_channel_t infrared_rx_channel = CONFIG_EXAMPLE_RMT_RX_CHANNEL;
static ir_code_t ir_code = {0, 0, 0, false, false};
static RingbufHandle_t rb = NULL;
static ir_builder_t *ir_builder = NULL;
static ir_parser_t *ir_parser = NULL;

static void driver_ir_tx_init()
{
    rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX(CONFIG_EXAMPLE_RMT_TX_GPIO, infrared_tx_channel);
    rmt_tx_config.tx_config.carrier_en = true;
    rmt_config(&rmt_tx_config);
    rmt_driver_install(infrared_tx_channel, 0, 0);
    ir_builder_config_t ir_builder_config = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)infrared_tx_channel);
    ir_builder_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
#if CONFIG_EXAMPLE_IR_PROTOCOL_NEC
    ir_builder = ir_builder_rmt_new_nec(&ir_builder_config);
#elif CONFIG_EXAMPLE_IR_PROTOCOL_RC5
    ir_builder = ir_builder_rmt_new_rc5(&ir_builder_config);
#endif
}

static void driver_ir_rx_init()
{
    rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(CONFIG_EXAMPLE_RMT_RX_GPIO, infrared_rx_channel);
    rmt_config(&rmt_rx_config);
    rmt_driver_install(infrared_rx_channel, 1000, 0);
    ir_parser_config_t ir_parser_config = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)infrared_rx_channel);
    ir_parser_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
#if CONFIG_EXAMPLE_IR_PROTOCOL_NEC
    ir_parser = ir_parser_rmt_new_nec(&ir_parser_config);
#elif CONFIG_EXAMPLE_IR_PROTOCOL_RC5
    ir_parser = ir_parser_rmt_new_rc5(&ir_parser_config);
#endif
    //get RMT RX ringbuffer
    rmt_get_ringbuf_handle(infrared_rx_channel, &rb);
    assert(rb != NULL);
    // Start receive
    rmt_rx_start(infrared_rx_channel, true);
}

void driver_ir_task(void *arg)
{
    uint32_t addr = 0;
    uint32_t cmd = 0;
    size_t length = 0;
    bool repeat = false;
    rmt_item32_t *items = NULL;
    driver_ir_tx_init();
    driver_ir_rx_init();
    while (true)
    {
        items = (rmt_item32_t *) xRingbufferReceive(rb, &length, portMAX_DELAY);
        if (items)
        {
            length /= 4; // one RMT = 4 Bytes
            if (ir_parser->input(ir_parser, items, length) == ESP_OK)
            {
                if (ir_parser->get_scan_code(ir_parser, &addr, &cmd, &repeat) == ESP_OK)
                {
                    ir_code.data_valid = true;
                    ir_code.addr = addr;
                    ir_code.cmd = cmd;
                    ir_code.repeat = repeat;
                    ir_code.time_tick = esp_timer_get_time() / 1000;
                    // ESP_LOGI(TAG, "Scan Code %s --- addr: 0x%04x cmd: 0x%04x", repeat ? "(repeat)" : "", addr, cmd);
                }
            }
            //after parsing the data, return spaces to ringbuffer.
            vRingbufferReturnItem(rb, (void *) items);
        }
    }
}

void ir_code_update(void)
{
    uint64_t time_tick = esp_timer_get_time() / 1000;
    if((ir_code.data_valid) && (time_tick -  ir_code.time_tick > 200))
    {
        ir_code.data_valid = false;
        ir_code.addr = 0x0000;
        ir_code.cmd = 0x0000;
        ir_code.repeat = false;
    }
}

void send_ir_code(uint32_t addr, uint32_t cmd)
{
    rmt_item32_t *items = NULL;
    size_t length = 0;

    // ESP_LOGI(TAG, "Send command 0x%x to address 0x%x", cmd, addr);
    // Send new key code
    ESP_ERROR_CHECK(ir_builder->build_frame(ir_builder, addr, cmd));
    ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &items, &length));
    //To send data according to the waveform items.
    rmt_write_items(infrared_tx_channel, items, length, false);
    // Send repeat code
    vTaskDelay(pdMS_TO_TICKS(ir_builder->repeat_period_ms));
    ESP_ERROR_CHECK(ir_builder->build_repeat_frame(ir_builder));
    ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &items, &length));
    rmt_write_items(infrared_tx_channel, items, length, false);
}

ir_code_t get_ir_code(void)
{
    return ir_code;
}