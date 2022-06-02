#ifndef __DRV_INFRARED_TRANSCEIVER_H__
#define __DRV_INFRARED_TRANSCEIVER_H__

#include "esp_err.h"

#define CONFIG_EXAMPLE_IR_PROTOCOL_NEC   1
#define CONFIG_EXAMPLE_RMT_TX_CHANNEL    0
#define CONFIG_EXAMPLE_RMT_RX_CHANNEL    4
#define CONFIG_EXAMPLE_RMT_TX_GPIO       42
#define CONFIG_EXAMPLE_RMT_RX_GPIO       40

#define INFRARED_TRANSCEIVER_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 2)
#define INFRARED_TRANSCEIVER_TASK_STACK_SIZE      (8 * 1024)

// extern void infrared_transceiver_init(void);
// extern void infrared_transceiver_update(void);
// extern float get_infrared_receive_data(uint8_t ir_channel);
// extern void get_infrared_code(uint8_t ir_channel);

typedef struct
{
    uint32_t addr;
    uint32_t cmd;
    uint64_t time_tick;
    bool repeat;
    bool data_valid;
} ir_code_t;

extern void  driver_ir_task(void *arg);
extern void ir_code_update(void);
extern void send_ir_code(uint32_t addr, uint32_t cmd);
extern ir_code_t get_ir_code(void);

#endif /* __DRV_INFRARED_TRANSCEIVER_H__ */ 
