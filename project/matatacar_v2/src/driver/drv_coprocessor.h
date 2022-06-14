#ifndef __DRV_COPROCESSOR_H__
#define __DRV_COPROCESSOR_H__

#include "esp_err.h"

// Task
#define DRV_COPROCESSOR_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 4)
#define DRV_COPROCESSOR_TASK_STACK_SIZE      (4 * 1024)
#define DRV_COPROCESSOR_POLLING_TIME         5

union{
  uint8_t byteVal[2];
  uint16_t shortVal;
}val2byte;

typedef struct
{
	bool is_auto_color;
	uint16_t ir_threshold;
    uint16_t r_value;
    uint16_t g_value;
    uint16_t b_value;
    uint16_t color_sensor_off;
    uint16_t left_ir;
    uint16_t right_ir;

}sensor_data_type;

sensor_data_type sensor_value;

void drv_coprpcessor_init(void);
void drv_coprpcessor_printf(char *fmt,...);
void drv_coprpcessor_task(void *arg);

#endif /* __DRV_COPROCESSOR_H__ */ 
