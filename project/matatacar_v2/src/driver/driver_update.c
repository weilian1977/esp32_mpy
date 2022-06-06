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

#include "drv_i2c.h"
#if MODULE_I2S_MIC_ENABLE
#include "mt_i2s_mic.h"
#endif /* MODULE_I2S_MIC_ENABLE */

#if MODULE_EVENT_ENABLE
#include "mt_event_mechanism.h"
#endif /* MODULE_EVENT_ENABLE */

#include "driver_update.h"
#include "mt_esp32_button.h"
#include "drv_button.h"
#include "drv_light_sensor.h"

/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define   TAG                         ("DRIVER_UPDATE")

#define BUTTON_NUM            (3)

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
  i2c_master_init(I2C_NUM_0);
}

void driver_update_t(void)
{

#if MODULE_PIN_ENABLE
  mt_esp32_pin_value_update_t();
#endif
}

//#define EVE_PARAMETER_BYTE_MAX_SIZE (64)
#if MODULE_EVENT_ENABLE
void driver_event_listenning(void)
{
  uint8_t para[EVE_PARAMETER_BYTE_MAX_SIZE];
#if MODULE_BUTTON_ENABLE
  for (uint8_t i = 0; i < BUTTON_NUM; i++)
  {
    if (is_key_pressed(BUTTON_A + i))
    {
      para[0] = 1;
      mt_eve_trigger_by_type_t(EVENT_BUTTON_A_PRESSED + i*2, para);
    }
    else
    {
      para[0] = 0;
      mt_eve_trigger_by_type_t(EVENT_BUTTON_A_PRESSED + i*2, para);
    }
    if (is_key_released(BUTTON_A + i))
    {
      para[0] = 1;
      mt_eve_trigger_by_type_t(EVENT_BUTTON_A_RELEASED + i*2, para);
    }
    else
    {
      para[0] = 0;
      mt_eve_trigger_by_type_t(EVENT_BUTTON_A_RELEASED + i*2, para);
    }
  }
  float temp = get_light_value(1);
  memcpy(para, &temp, sizeof(float)); 
  mt_eve_trigger_by_type_t(EVENT_LIGHT_LEFT_LESS, para);
  mt_eve_trigger_by_type_t(EVENT_LIGHT_LEFT_MORE, para);
  temp = get_light_value(0);
  memcpy(para, &temp, sizeof(float)); 
  mt_eve_trigger_by_type_t(EVENT_LIGHT_RIGHT_LESS, para);
  mt_eve_trigger_by_type_t(EVENT_LIGHT_RIGHT_MORE, para);
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
  //driver_init_t();
  xTaskCreatePinnedToCore(driver_update_task_t, "driver_update_task", 4 * 1024, NULL, 0, NULL, 0);
}
/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
