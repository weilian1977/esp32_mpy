#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#include "mt_err.h"


#include "esp_system.h"
#include "esp_task.h"
#include "soc/cpu.h"

#include "esp_log.h"

#include "mt_module_config.h"
//#include "project_config.h"

//#include "driver.h"
#if MODULE_I2S_MIC_ENABLE
#include "mt_i2s_mic.h"
#endif /* MODULE_I2S_MIC_ENABLE */

#if MODULE_EVENT_ENABLE
#include "mt_event_mechanism.h"
#endif /* MODULE_EVENT_ENABLE */

#include "driver_update.h"
#include "mt_esp32_button.h"
#include "drv_qmi8658.h"

/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define   TAG                         ("DRIVER_UPDATE")

#define BUTTON_NUM            (1)

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/

/******************************************************************************
 DEFINE PRIVATE DATAS
 ******************************************************************************/
 
/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
/* system */ 
void driver_init_t(void)
{  
#if MODULE_PIN_ENABLE
  mt_esp32_pin_init_t();
#endif

#if MODULE_GYRO_ENABLE
  mt_esp32_gyro_init_t();
  mt_esp32_gyro_event_init_t();
#endif

#if MODULE_BUTTON_ENABLE
  mt_esp32_button_init_t();
#endif

}

void driver_update_t(void)
{

#if MODULE_PIN_ENABLE
  mt_esp32_pin_value_update_t();
#endif

#if MODULE_GYRO_ENABLE
  mt_esp32_gyro_update_t();
  //get_acc_gyro_angle();
#endif

#if MODULE_BUTTON_ENABLE
  mt_esp32_button_update_t();
#endif
}

//#define EVE_PARAMETER_BYTE_MAX_SIZE (64)
#if MODULE_EVENT_ENABLE
void driver_event_listenning(void)
{
  uint8_t para[EVE_PARAMETER_BYTE_MAX_SIZE];
#if MODULE_GYRO_ENABLE
  //mt_esp32_gyro_is_shaked_t((bool *)&para[0]);
  //mt_eve_trigger_by_type_t(EVENT_SHAKED, para);

  mt_esp32_gyro_get_tilt_status_t(TILT_LEFT, (uint16_t *)&para[0]);
  mt_eve_trigger_by_type_t(EVENT_TILT_LEFT, para);

  mt_esp32_gyro_get_tilt_status_t(TILT_RIGHT, (uint16_t *)&para[0]);
  mt_eve_trigger_by_type_t(EVENT_TILT_RIGHT, para);

  mt_esp32_gyro_get_tilt_status_t(TILT_FORWARD, (uint16_t *)&para[0]);
  mt_eve_trigger_by_type_t(EVENT_TILT_FORWARD, para);

  mt_esp32_gyro_get_tilt_status_t(TILT_BACK, (uint16_t *)&para[0]);
  mt_eve_trigger_by_type_t(EVENT_TILT_BACKWARD, para);
#endif

#if MODULE_BUTTON_ENABLE
  for(uint8_t i = 0; i < BUTTON_NUM; i++)
  {
    mt_esp32_button_is_pressed_t(i, (bool *)&para[0]);
    mt_eve_trigger_by_type_t(EVENT_BUTTON + i, para);

  }
#endif

  //float temp = (float)get_timer_value_second_t(0);
  //memcpy(para, &temp, sizeof(float)); 
  //mt_eve_trigger_by_type_t(EVE_TIME_OVER, para);

}
#endif /* MODULE_EVENT_ENABLE */ 


void driver_update_task_t(void *parameter)
{
  while(1)
  {
    driver_update_t();
#if MODULE_EVENT_ENABLE
    driver_event_listenning();
#endif /* MODULE_EVENT_ENABLE */
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

//xTaskCreatePinnedToCore(get_cmd_data, "get_cmd_data", 4 * 1024, NULL, MP_TASK_PRIORITY, &mp_main_task_handle, MP_TASK_COREID);

void driver_update_task_init(void)
{
  ESP_LOGI(TAG, "button false");
  driver_init_t();
  xTaskCreatePinnedToCore(driver_update_task_t, "driver_update_task", 4 * 1024, NULL, 0, NULL, 0);
}
/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
