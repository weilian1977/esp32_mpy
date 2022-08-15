#ifndef __DRV_BUTTON_H__
#define __DRV_BUTTON_H__

#include "esp_err.h"

#define BUTTON_A       1
#define BUTTON_B       2
#define BUTTON_PLAY    3

//1- circular 2- square 3-triangle
#define BUTTON_CICULAR  1
#define BUTTON_SQUARE   2
#define BUTTON_TRIANGLE 3

extern uint8_t get_button_value(void);
extern bool is_key_pressed(uint8_t button_id);
extern bool is_key_released(uint8_t button_id);

#endif /* __DRV_BUTTON_H__ */ 
