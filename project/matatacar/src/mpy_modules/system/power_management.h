#ifndef __POWER_MANAGEMENT_H__
#define __POWER_MANAGEMENT_H__

#include "esp_err.h"

#define MININUM_BATTERY_VOLTAGE      3.40f
#define LOW_POWER_ALARM_VOLTAGE      3.55f

#define POWER_PIN                    (39)
#define SW_PIN                       (27)

void power_pin_init(void);
bool get_power_pin_state(void);
void power_on(void);
void power_off(void);
void power_management(void);

#endif /* __POWER_MANAGEMENT_H__ */ 
