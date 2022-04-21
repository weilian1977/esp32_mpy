#ifndef __POWER_MANAGEMENT_H__
#define __POWER_MANAGEMENT_H__

#include "esp_err.h"

#define MININUM_BATTERY_VOLTAGE      3.42f
#define LOW_POWER_ALARM_VOLTAGE      3.65f

void power_management(void);

#endif /* __POWER_MANAGEMENT_H__ */ 
