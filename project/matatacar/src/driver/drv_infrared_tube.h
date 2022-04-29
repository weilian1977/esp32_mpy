#ifndef __DRV_INFRARED_TUBE_H__
#define __DRV_INFRARED_TUBE_H__

#include "esp_err.h"

//INFRARED_TUBE_OBSTACLE_AVOIDANCE
#define INFRARED_TUBE_OBSTACLE_AVOIDANCE       (ADC1_CHANNEL_3)        //GPIO4
#define INFRARED_TUBE_LINE_FOLLOWER_1          (ADC1_CHANNEL_4)        //GPIO5
#define INFRARED_TUBE_LINE_FOLLOWER_2          (ADC1_CHANNEL_5)        //GPIO6
#define INFRARED_TUBE_SENSOR_MAXNUM            3

extern void infrared_tube_sensor_init(void);
extern void infrared_tube_sensor_voltage_update(void);
extern float get_infrared_tube_value(uint8_t ir_channel);

#endif /* __DRV_INFRARED_TUBE_H__ */ 
