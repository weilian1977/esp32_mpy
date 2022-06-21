#ifndef __DRV_COPROCESSOR_H__
#define __DRV_COPROCESSOR_H__

#include "esp_err.h"

// Task
#define DRV_COPROCESSOR_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 4)
#define DRV_COPROCESSOR_TASK_STACK_SIZE      (4 * 1024)
#define DRV_COPROCESSOR_POLLING_TIME         5

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef enum
{
    COLOR_BLACK = 0,
    COLOR_WHITE,
    COLOR_GREY,
    COLOR_RED,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_BLUE,
    COLOR_PURPLE,
    COLOR_UNKNOWN,
} color_id_t;

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

uint16_t color_red_cali;
uint16_t color_green_cali;
uint16_t color_blue_cali;
uint16_t color_bri_ratio_cali;

void drv_coprpcessor_init(void);
void drv_coprpcessor_printf(char *fmt,...);
void drv_coprpcessor_task(void *arg);
esp_err_t get_color_sensor_calibration_value(void);
uint8_t drv_get_color_id(void);

#endif /* __DRV_COPROCESSOR_H__ */ 
