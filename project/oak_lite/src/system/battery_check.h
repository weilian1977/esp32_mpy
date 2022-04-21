#ifndef __BATTERY_CHECK_H__
#define __BATTERY_CHECK_H__

#include "esp_err.h"
#include "driver/adc.h"

#define BATTERY_CHECK_CHANNEL               (ADC1_CHANNEL_0)
#define HARDWARE_VERSION_CHECK_CHANNEL      (ADC1_CHANNEL_0)
#define DEFAULT_VREF               (1100) 
#define BATTERY_VOLTAGE_DAMPING    ((180 * 1000.0f) / (100 + 180))


#define BATTERY_CAPACITY_TABLE_LEN (13)

typedef struct
{
  float battery_voltage;
  uint8_t battery_capacity;
}battery_capacity_table_t;

extern void battery_check_init(void);
extern void hardware_version_check_init(void);
extern void battery_voltage_update(void);
extern void hardware_version_voltage_update(void);
extern void battery_capacity_update(void);
extern float get_battery_voltage(void);
extern float get_hardware_version_voltage(void);
extern uint8_t get_battery_capacity(void);
#endif /* __BATTERY_CHECK_H__ */ 
