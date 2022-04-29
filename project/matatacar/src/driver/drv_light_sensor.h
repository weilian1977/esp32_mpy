#ifndef __DRV_LIGHT_SENSOR_H__
#define __DRV_LIGHT_SENSOR_H__

#include "esp_err.h"

#define LIGHT_SENSOR_CHANNEL_1     (ADC1_CHANNEL_2)        //GPIO3
#define LIGHT_SENSOR_CHANNEL_2     (ADC2_CHANNEL_5)        //GPIO16
#define LIGHT_SENSOR_MAXNUM        2

extern void light_sensor_init(void);
extern void light_sensor_voltage_update(void);
extern float get_light_value(uint8_t light_channel);

#endif /* __DRV_LIGHT_SENSOR_H__ */ 
