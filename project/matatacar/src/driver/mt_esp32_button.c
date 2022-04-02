#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "esp_log.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_err.h"

#include "mt_module_config.h"
#include "mt_esp32_button.h"

#include "esp_log.h"

#if MODULE_BUTTON_ENABLE
/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define BUTTON_MAX         (8)

#ifndef BUTTON_NUM
  #define BUTTON_NUM         (1)
#endif

#if BUTTON_NUM > BUTTON_MAX 
  #undef BUTTON_NUM
  #define BUTTON_NUM         BUTTON_MAX
#endif

#ifndef PRODUCT_BUTTON_IO_0
  #define BUTTON_IO_0 (48)
  #define BUTTON_0_DEFAULT_LEVEL (1)
#else
  #define BUTTON_IO_0 PRODUCT_BUTTON_IO_0
  #define BUTTON_0_DEFAULT_LEVEL PRODUCT_BUTTON_0_DEFAULT_LEVEL
#endif
#ifndef PRODUCT_BUTTON_IO_1
  #define BUTTON_IO_1 (-1)
  #define BUTTON_1_DEFAULT_LEVEL (0)
#else
  #define BUTTON_IO_1 PRODUCT_BUTTON_IO_1
  #define BUTTON_1_DEFAULT_LEVEL PRODUCT_BUTTON_1_DEFAULT_LEVEL
#endif
#ifndef PRODUCT_BUTTON_IO_2
  #define BUTTON_IO_2 (-1)
  #define BUTTON_2_DEFAULT_LEVEL (0)
#else
  #define BUTTON_IO_2 PRODUCT_BUTTON_IO_2
  #define BUTTON_2_DEFAULT_LEVEL PRODUCT_BUTTON_2_DEFAULT_LEVEL
#endif
#ifndef PRODUCT_BUTTON_IO_3
  #define BUTTON_IO_3 (-1)
  #define BUTTON_3_DEFAULT_LEVEL (0)
#else
  #define BUTTON_IO_3 PRODUCT_BUTTON_IO_3
  #define BUTTON_3_DEFAULT_LEVEL PRODUCT_BUTTON_3_DEFAULT_LEVEL
#endif
#ifndef PRODUCT_BUTTON_IO_4
  #define BUTTON_IO_4 (-1)
  #define BUTTON_4_DEFAULT_LEVEL (0)
#else
  #define BUTTON_IO_4 PRODUCT_BUTTON_IO_4
  #define BUTTON_4_DEFAULT_LEVEL PRODUCT_BUTTON_4_DEFAULT_LEVEL
#endif
#ifndef PRODUCT_BUTTON_IO_5
  #define BUTTON_IO_5 (-1)
  #define BUTTON_5_DEFAULT_LEVEL (0)
#else
  #define BUTTON_IO_5 PRODUCT_BUTTON_IO_5
  #define BUTTON_5_DEFAULT_LEVEL PRODUCT_BUTTON_5_DEFAULT_LEVEL
#endif
#ifndef PRODUCT_BUTTON_IO_6
  #define BUTTON_IO_6 (-1)
  #define BUTTON_6_DEFAULT_LEVEL (0)
#else
  #define BUTTON_IO_6 PRODUCT_BUTTON_IO_6
  #define BUTTON_6_DEFAULT_LEVEL PRODUCT_BUTTON_6_DEFAULT_LEVEL
#endif
#ifndef PRODUCT_BUTTON_IO_7
  #define BUTTON_IO_7 (-1)
  #define BUTTON_7_DEFAULT_LEVEL (0)
#else
  #define BUTTON_IO_7 PRODUCT_BUTTON_IO_7
  #define BUTTON_7_DEFAULT_LEVEL PRODUCT_BUTTON_7_DEFAULT_LEVEL
#endif

/************************************************************/
#define TAG                           ("mt_button")

#define SECOND_TRIGGER_INTERVAL       (700) 
#define THIRD_OVER_TRIGGER_INTERVAL   (100) 

#define BUTTON_ID_ERR_STRING  "button id wrong"

#define BUTTON_ID_CHECK(a, str, ret_val) 
    //if(a >= BUTTON_NUM) 
    //{ 
      //MT_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str); 
      //return (ret_val); 
    //}

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/  
typedef struct
{
  mt_module_status_t module_button_status;
  uint64_t button_io[BUTTON_MAX];
  uint8_t button_default_level[BUTTON_MAX];
  uint8_t button_current_level[BUTTON_MAX];
  uint16_t all_status;
}mt_esp32_button_structure_t;

/******************************************************************************
 DECLARE PRIVATE DATAS
 ******************************************************************************/
mt_esp32_button_structure_t mt_esp32_button_structure;

#define MT_ESP32_MODULE_BUTTON_STATUS_CHECK(a, str, ret_val) 
    //if((a) != mt_esp32_button_structure.module_button_status) 
    //{ 
      //MT_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str); 
      //return (ret_val); 
    //}
/******************************************************************************
 DECLARE PUBLIC FUNCTIONS  
 ******************************************************************************/


/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
static mt_err_t mt_esp32_button_config_t(void) ;

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
mt_err_t mt_esp32_button_init_t(void)
{
  /* modify this function to config the button io and other informations */
  mt_esp32_button_structure.button_io[0] = BUTTON_IO_0;
  mt_esp32_button_structure.button_default_level[0] = BUTTON_0_DEFAULT_LEVEL;
  
  for(uint8_t i = 0; i < BUTTON_NUM; i++ )
  {
    mt_esp32_button_structure.all_status = 0;
  }
  mt_esp32_button_config_t();
  return MT_OK;
}

mt_err_t mt_esp32_button_get_level_t(uint8_t button_id, uint8_t *level)
{
  BUTTON_ID_CHECK(button_id, BUTTON_ID_ERR_STRING, MT_FAIL);
  (*level) = mt_esp32_button_structure.button_current_level[button_id];
  return MT_OK;
}

mt_err_t mt_esp32_button_is_pressed_t(uint8_t button_id, bool *sta)
{
  BUTTON_ID_CHECK(button_id, BUTTON_ID_ERR_STRING, MT_FAIL);
  if(mt_esp32_button_structure.button_current_level[button_id] == mt_esp32_button_structure.button_default_level[button_id])
  {
    (*sta) = false;
    //ESP_LOGI(TAG, "button false");
  }
  else
  {
    (*sta) = true;
    //ESP_LOGI(TAG, "button true");
  }
  return MT_OK;
}

mt_err_t mt_esp32_button_is_released_t(uint8_t button_id, bool *sta)
{
  BUTTON_ID_CHECK(button_id, BUTTON_ID_ERR_STRING, MT_FAIL);
  if(mt_esp32_button_structure.button_current_level[button_id] == mt_esp32_button_structure.button_default_level[button_id])
  {
    (*sta) = true;
  }
  else
  {
    (*sta) = false;
  }
  return MT_OK;
}

/* this function will update the mt_esp32_button_structure once */
mt_err_t mt_esp32_button_update_t(void)
{
  MT_ESP32_MODULE_BUTTON_STATUS_CHECK(MT_MODULE_INITIALIZED, "module button not init", MT_FAIL);
  for(uint8_t i = 0; i < BUTTON_NUM; i++)
  {
    uint8_t value = (uint8_t)(gpio_get_level(0));
    //ESP_LOGI(TAG, "value: %d", value);
    mt_esp32_button_structure.button_current_level[i] = (uint8_t)(gpio_get_level(mt_esp32_button_structure.button_io[i]));
    if(mt_esp32_button_structure.button_current_level[i])
    {
      mt_esp32_button_structure.all_status |= (1 << i);
    }
    else
    {
      mt_esp32_button_structure.all_status &= (~(1 << i));
    }
  }
  return MT_OK;
}

mt_err_t mt_esp32_button_test_t(void)
{
  bool sta = false;
  FUNCTION_RET_CHECK(mt_esp32_button_init_t(), "button unit test, init error", MT_FAIL);
  while(1)
  {
    mt_esp32_button_update_t();
    for(uint8_t i = 0; i < BUTTON_NUM; i++)
    {
      mt_esp32_button_is_pressed_t(i, &sta);
      ESP_LOGI(TAG, "button%d: %s", i, sta ? "pressed" : "released");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
static mt_err_t mt_esp32_button_config_t(void)  
{
  gpio_set_direction(0, GPIO_MODE_INPUT);
  mt_esp32_button_structure.module_button_status = MT_MODULE_INITIALIZED;
  return MT_OK;
}

#endif /* MODULE_BUTTON_ENABLE */