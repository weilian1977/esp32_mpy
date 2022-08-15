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
#include "drv_infrared_tube.h"
#include "drv_coprocessor.h"
#include "drv_infrared_transceiver.h"
#include "mt_tof.h"
#include "i2s_mic.h"

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
  get_color_sensor_calibration_value();
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
#endif
#if MODULE_LIGHT_SENSOR_ENABLE
  float temp = get_light_value(1);
  temp *= 100.0 / 3.3;
  memcpy(para, &temp, sizeof(float)); 
  mt_eve_trigger_by_type_t(EVENT_LIGHT_LEFT_LESS, para);
  mt_eve_trigger_by_type_t(EVENT_LIGHT_LEFT_MORE, para);
  temp = get_light_value(0);
  temp *= 100.0 / 3.3;
  memcpy(para, &temp, sizeof(float)); 
  mt_eve_trigger_by_type_t(EVENT_LIGHT_RIGHT_LESS, para);
  mt_eve_trigger_by_type_t(EVENT_LIGHT_RIGHT_MORE, para);
#endif

#if MODULE_TOF_ENABLE
  float tof_distance = 0;
  mt_tof_get_distance_t(&tof_distance);
  if(tof_distance < 250.0f)
  {
    para[0] = 1;
    mt_eve_trigger_by_type_t(EVENT_OBSTACLE_DETECTED, para);
  }else{
    para[0] = 0;
    mt_eve_trigger_by_type_t(EVENT_OBSTACLE_DETECTED, para);
  }
#endif

#if MODULE_COLOR_SENSOR_ENABLE
  uint8_t color_id_value = drv_get_color_id();
  switch(color_id_value)
  {
    case COLOR_BLACK:
      strcpy((char*)para, "black");
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_WHITE:
      strcpy((char*)para, "white");
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_GREY:
      strcpy((char*)para, "grey"); 
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_RED:
      strcpy((char*)para, "red");
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_ORANGE:
      strcpy((char*)para, "orange");
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_YELLOW:
      strcpy((char*)para, "yellow");
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_GREEN:
      strcpy((char*)para, "green");
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_CYAN:
      strcpy((char*)para, "cyan");
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_BLUE:
      strcpy((char*)para, "blue");
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_PURPLE:
      strcpy((char*)para, "purple");
      mt_eve_trigger_by_type_t(EVENT_COLOR_DETECTED, para);
      break;
    case COLOR_UNKNOWN:
      break;
  }
#endif

  float volume_value;
  get_loudness_value(average_value_type, &volume_value);
  memcpy(para, &volume_value, sizeof(float)); 
  mt_eve_trigger_by_type_t(EVENT_MICROPHONE_LESS, para);
  mt_eve_trigger_by_type_t(EVENT_MICROPHONE_MORE, para);

  ir_code_t ir_code_data;
  ir_code_data = get_ir_code();
  int32_t cmd = ir_code_data.cmd;
  switch(cmd)
  {
    case 0xF708:
      strcpy((char*)para, "1");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xE31C:
      strcpy((char*)para, "2");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xA55A:
      strcpy((char*)para, "3");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xBD42:
      strcpy((char*)para, "4");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xAD52:
      strcpy((char*)para, "5");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xB54A:
      strcpy((char*)para, "6");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xBA45:
      strcpy((char*)para, "volume");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xB946:
      strcpy((char*)para, "mode");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xB847:
      strcpy((char*)para, "speed");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xF30C:
      strcpy((char*)para, "dance");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xE718:
      strcpy((char*)para, "led");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xA15E:
      strcpy((char*)para, "music");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xBF40:
      strcpy((char*)para, "forward");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xE619:
      strcpy((char*)para, "backward");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xF807:
      strcpy((char*)para, "left");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xF609:
      strcpy((char*)para, "right");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0xEA15:
      strcpy((char*)para, "play");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
    case 0:
      strcpy((char*)para, "Unknown");
      mt_eve_trigger_by_type_t(EVENT_MESSAGE, para);
      break;
  }

  //float temp = (float)get_timer_value_second_t(0);
  //memcpy(para, &temp, sizeof(float)); 
  //mt_eve_trigger_by_type_t(EVE_TIME_OVER, para);

}
#endif /* MODULE_EVENT_ENABLE */ 


void driver_update_task_t(void *parameter)
{
  get_color_sensor_calibration_value();
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
  //driver_init_t();
  xTaskCreatePinnedToCore(driver_update_task_t, "driver_update_task", 4 * 1024, NULL, 0, NULL, 0);
}
/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
