#ifndef __SYSTEM_MANAGEMENT_H__
#define __SYSTEM_MANAGEMENT_H__

#include "esp_err.h"

// Task
#define SYSTEM_MANAGEMENT_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 4)
#define SYSTEM_MANAGEMENT_TASK_STACK_SIZE      (4 * 1024)
#define SYSTEM_POLLING_TIME                    10

#define SYSTEM_GD32_NRST_PIN                   (32)

void system_gd32_init(void);
void system_management_task(void *pvParameter);
#endif /* __SYSTEM_MANAGEMENT_H__ */ 
