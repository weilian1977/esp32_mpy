#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "esp_log.h"
#include "battery_check.h"
#include "firmware_version.h"

/********************************************************************
 DEFINE MACROS
 ********************************************************************/
#define TAG   "FIRMWARE_VERSION"

/********************************************************************
 DEFINE PRIVATE DATAS
 ********************************************************************/

/********************************************************************
 DEFINE PUBLIC DATAS
 ********************************************************************/
static char s_firmware_version[FIRMWARE_VERSION_LENGTH_MAX] = FIREFLY_VERSION;

/********************************************************************
 DEFINE PUBLIC FUNCTIONS
 ********************************************************************/
const char *get_firmware_version_t(void)
{
  if(strlen(s_firmware_version) > FIRMWARE_VERSION_LENGTH_MAX)
  {
  	s_firmware_version[FIRMWARE_VERSION_LENGTH_MAX - 1] = '\0';
  }
  return s_firmware_version;
}

uint8_t get_hardware_version_t(void)
{
  float hardware_version_voltage;
  uint8_t hardware_version = 0;
  hardware_version_voltage = get_hardware_version_voltage();
  if(hardware_version_voltage < 0.15f)
  {
    hardware_version = 0;
  }
  else if((hardware_version_voltage > 0.15f) && (hardware_version_voltage < (HARDWARE_VERSION_NUM1 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 1;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM2 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM2 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 2;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM3 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM3 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 3;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM4 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM4 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 4;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM5 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM5 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 5;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM6 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM6 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 6;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM7 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM7 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 7;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM8 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM8 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 8;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM9 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM9 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 9;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM10 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM10 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 10;
  }
  else if((hardware_version_voltage > (HARDWARE_VERSION_NUM11 - HARDWARE_VERSION_OFFSET)) && (hardware_version_voltage < (HARDWARE_VERSION_NUM11 + HARDWARE_VERSION_OFFSET)))
  {
    hardware_version = 11;
  }
  else if(hardware_version_voltage > 2.45f)
  {
    hardware_version = 12;
  }
  else
  {
    hardware_version = 255;
  }
  return hardware_version;
}

void show_firmware_version_t(void)
{
  printf("firmware version is:%s\n", s_firmware_version);
  printf("hardware version is:%d\n", get_hardware_version_t());
  printf("build time: %s, %s\n", __DATE__, __TIME__);
}

int firmware_version_append_t(char *str)
{
  int len = 0;
  len = strlen(str);

  if(len > (FIRMWARE_VERSION_LENGTH_MAX - 1))
  {
  	ESP_LOGE(TAG, "string is too long, %d, max is:%d", strlen(str), (FIRMWARE_VERSION_LENGTH_MAX - 1));
  	return -1;
  }
  if(strlen(s_firmware_version) + len > (FIRMWARE_VERSION_LENGTH_MAX - 1))
  {
  	ESP_LOGE(TAG, "version is too long:%d, max is:%d", strlen(s_firmware_version), (FIRMWARE_VERSION_LENGTH_MAX - 1));
  	return -1;
  }

  strcat(s_firmware_version, str);
  return 0;
}
