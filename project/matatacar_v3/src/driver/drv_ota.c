#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "nvs.h"

#include "esp_err.h"
#include "esp_log.h"
#include "firmware_version.h"
#include "drv_ota.h"

/******************************************************************************
 DEFIEN MACROS
 ******************************************************************************/
#define TAG                               ("drv_ota")
#define DEFAULT_WIFI_MODE_STA             (1) 

#define OTA_SET_ALL                       (0)
#define OTA_SET_VERSION                   (1)
#define OTA_SET_STA_SSID                  (2)
#define OTA_SET_STA_PASSWORD              (3)
#define OTA_SET_HOST                      (4)
#define OTA_SET_URL                       (5)

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/
#define ESP32_OTA_CONFIG_NV_NAMESPACE     ("ota_config")
#define ESP32_OTA_CONFIG_NV_VERSION       ("firefly_version")
#define ESP32_OTA_CONFIG_NV_STA_SSID      ("wifi_ssid")
#define ESP32_OTA_CONFIG_NV_STA_PASSWORD  ("wifi_password")
#define ESP32_OTA_CONFIG_NV_HOST          ("ota_host_name")
#define ESP32_OTA_CONFIG_NV_URL           ("ota_info_url")

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/  


/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
 esp32_wifi_ota_config_structure_t esp32_wifi_ota_config_structure;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
 
/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void esp32_ota_config_show(char *tag)
{
  printf("\n");
  printf("%s: ota config informaion\n", tag);
  printf("version is %s\n", esp32_wifi_ota_config_structure.info_t.esp32_firmware_version);
  printf("sta ssid is %s\n", esp32_wifi_ota_config_structure.info_t.sta_ssid);
  printf("sta password is %s\n", esp32_wifi_ota_config_structure.info_t.sta_password);
  printf("host is %s\n", esp32_wifi_ota_config_structure.info_t.ota_information_file_host);
  printf("url is %s\n", esp32_wifi_ota_config_structure.info_t.ota_information_file_url);
  printf("\n");
}

void esp32_ota_config_init(void)
{ 
  memset(&esp32_wifi_ota_config_structure, 0, sizeof(esp32_wifi_ota_config_structure_t));
  strcpy(esp32_wifi_ota_config_structure.info_t.esp32_firmware_version, get_firmware_version_t());
#if ESP32_WIFI_OTA_CONFIG_STORE_IN_NV
  if(esp32_ota_check_config_in_nv_flash() == -1)
  { 
    ESP_LOGE(TAG, "write new config");
    strcpy(esp32_wifi_ota_config_structure.info_t.sta_ssid, ESP32_OTA_DEFAULT_WIFI_SSID);
    strcpy(esp32_wifi_ota_config_structure.info_t.sta_password, ESP32_OTA_DEFAULT_WIFI_PASSWORD);
    strcpy(esp32_wifi_ota_config_structure.info_t.ota_information_file_host, ESP32_OTA_DEFAULT_WIFI_HOST);
    strcpy(esp32_wifi_ota_config_structure.info_t.ota_information_file_url, ESP32_OTA_DEFAULT_WIFI_URL);
    esp32_ota_write_config_info_to_nv_flash(OTA_SET_ALL);
  }
  else
  { 
    esp32_ota_write_config_info_to_nv_flash(OTA_SET_VERSION);
    esp32_ota_read_config_from_nv_flash();
    if(strcmp(esp32_wifi_ota_config_structure.info_t.esp32_firmware_version, get_firmware_version_t()) != 0)
    {
      ESP_LOGE(TAG, "ota config init error, current version and nvs version is not matched");
    }
  }
#else /* ESP32_WIFI_OTA_CONFIG_STORE_IN_NV */
/* save in filesystem */


#endif

}

void esp32_ota_config_set_wifi_mode(uint8_t mode)
{
  esp32_wifi_ota_config_structure.info_t.wifi_mode = mode;
  
#if ESP32_WIFI_OTA_CONFIG_STORE_IN_NV
  
#else /* ESP32_WIFI_OTA_CONFIG_STORE_IN_NV */

#endif /* ESP32_WIFI_OTA_CONFIG_STORE_IN_NV */
}

void esp32_ota_config_get_wifi_mode(uint8_t *mode)
{
  (*mode) = esp32_wifi_ota_config_structure.info_t.wifi_mode;
}

void esp32_ota_config_set_wifi_info(uint8_t item, const char * str)
{
  if(item == AP_SSID)
  {
    strcpy((esp32_wifi_ota_config_structure.info_t.ap_ssid), str);
  }
  else if(item == AP_PASSWORD)
  {
    strcpy(esp32_wifi_ota_config_structure.info_t.ap_password, str);
  }
  else if(item == STA_SSID)
  {
    strcpy(esp32_wifi_ota_config_structure.info_t.sta_ssid, str);
    esp32_ota_write_config_info_to_nv_flash(OTA_SET_STA_SSID);
  }
  else if(item == STA_PASSWORD)
  {
    strcpy(esp32_wifi_ota_config_structure.info_t.sta_password, str);
    esp32_ota_write_config_info_to_nv_flash(OTA_SET_STA_PASSWORD);
  }
  else if(item == OTA_INFO_HOST)
  {
    strcpy(esp32_wifi_ota_config_structure.info_t.ota_information_file_host, str);
    esp32_ota_write_config_info_to_nv_flash(OTA_INFO_HOST);
  }
  else if(item == OTA_INFO_URL)
  {
    strcpy(esp32_wifi_ota_config_structure.info_t.ota_information_file_url, str);
    esp32_ota_write_config_info_to_nv_flash(OTA_SET_URL);
  }
  else if(item == OTA_BIN_HOST)
  {
    strcpy(esp32_wifi_ota_config_structure.info_t.ota_update_source_file_host, str);
  }
  else if(item == OTA_BIN_HOST)
  {
    strcpy(esp32_wifi_ota_config_structure.info_t.ota_update_source_file_url, str);
  }
  ESP_LOGE(TAG, "item is %d, string is %s", item, str);
}

void esp32_ota_config_get_wifi_info(uint8_t item, char* const str)
{
  if(item == AP_SSID)
  {
    strcpy(str, (esp32_wifi_ota_config_structure.info_t.ap_ssid));
  }
  else if(item == AP_PASSWORD)
  {
    strcpy(str, esp32_wifi_ota_config_structure.info_t.ap_password);
  }
  else if(item == STA_SSID)
  {
    strcpy(str, esp32_wifi_ota_config_structure.info_t.sta_ssid);
  }
  else if(item == STA_PASSWORD)
  {
    strcpy(str, esp32_wifi_ota_config_structure.info_t.sta_password);
  }
  else if(item == OTA_INFO_HOST)
  {
    strcpy(str, esp32_wifi_ota_config_structure.info_t.ota_information_file_host);
  }
  else if(item == OTA_INFO_URL)
  {
    strcpy(str, esp32_wifi_ota_config_structure.info_t.ota_information_file_url);
  }
  else if(item == OTA_BIN_HOST)
  {
    strcpy(str, esp32_wifi_ota_config_structure.info_t.ota_update_source_file_host);
  }
  else if(item == OTA_BIN_URL)
  {
    strcpy(str, esp32_wifi_ota_config_structure.info_t.ota_update_source_file_url);
  }
  
}

int esp32_ota_check_config_in_nv_flash(void)
{
  esp_err_t err;
  nvs_handle my_handle;
  err = nvs_open(ESP32_OTA_CONFIG_NV_NAMESPACE, NVS_READONLY, &my_handle);
  if(err != ESP_OK)
  {
    ESP_LOGE(TAG, "Error (%d) opening NVS handle!", err);
    nvs_close(my_handle);
    return -1;
  }
  else
  {
    ESP_LOGE(TAG, "found ota config in nv flash");
    nvs_close(my_handle);
    return 0;
  } 
}

int esp32_ota_write_config_info_to_nv_flash(uint8_t item)
{
  esp_err_t err;
  nvs_handle my_handle;
  int res = 0;
  size_t len = esp32_WIFI_OTA_URL_LENGTH_MAX;
  char check_buffer[esp32_WIFI_OTA_URL_LENGTH_MAX];
  memset(check_buffer, 0, esp32_WIFI_OTA_URL_LENGTH_MAX);
  uint8_t retry_time = 0;
wtite_start:    
  err = nvs_open(ESP32_OTA_CONFIG_NV_NAMESPACE, NVS_READWRITE, &my_handle);
  if(err != ESP_OK)
  {
    ESP_LOGE(TAG, "Error (%d) opening NVS handle!", err);
    res = -1;
    return res;
  } 
  else 
  {
    ESP_LOGI(TAG, "nv open successed");
    // Write
    if(item == OTA_SET_VERSION || item == OTA_SET_ALL)
    {   
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_VERSION, check_buffer, &len);
      if(strcmp(get_firmware_version_t(), check_buffer) != 0)
      {
        err |= nvs_set_str(my_handle, ESP32_OTA_CONFIG_NV_VERSION, get_firmware_version_t());
      }
      else
      {
        ESP_LOGE(TAG, "version the same");
      }
    }
    if(item == OTA_SET_STA_SSID || item == OTA_SET_ALL)
    {
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_STA_SSID, check_buffer, &len);
      if(strcmp(esp32_wifi_ota_config_structure.info_t.sta_ssid, check_buffer) != 0)
      {
        err |= nvs_set_str(my_handle, ESP32_OTA_CONFIG_NV_STA_SSID, esp32_wifi_ota_config_structure.info_t.sta_ssid);
      }
      else
      {
        ESP_LOGE(TAG, "ssid the same");
      }
    }
    if(item == OTA_SET_STA_PASSWORD || item == OTA_SET_ALL)
    {
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_STA_PASSWORD, check_buffer, &len);
      
      if(strcmp(esp32_wifi_ota_config_structure.info_t.sta_password, check_buffer) != 0)
      {
        err |= nvs_set_str(my_handle, ESP32_OTA_CONFIG_NV_STA_PASSWORD, esp32_wifi_ota_config_structure.info_t.sta_password);
      }
      else
      {
        ESP_LOGE(TAG, "pass the same");
      }
    }
    if(item == OTA_SET_HOST || item == OTA_SET_ALL)
    {
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_HOST, check_buffer, &len);
      if(strcmp(esp32_wifi_ota_config_structure.info_t.ota_information_file_host, check_buffer) != 0)
      {
        err |= nvs_set_str(my_handle, ESP32_OTA_CONFIG_NV_HOST, esp32_wifi_ota_config_structure.info_t.ota_information_file_host);
      }
      else
      {
        ESP_LOGE(TAG, "host the same");
      }
    }
    if(item == OTA_SET_URL || item == OTA_SET_ALL)
    {
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_URL, check_buffer, &len);
      if(strcmp(esp32_wifi_ota_config_structure.info_t.ota_information_file_url, check_buffer) != 0)
      {
        err |= nvs_set_str(my_handle, ESP32_OTA_CONFIG_NV_URL, esp32_wifi_ota_config_structure.info_t.ota_information_file_url);
      }
      else
      {
        ESP_LOGE(TAG, "url the same");
      }
    }
    
    if(err != ESP_OK)
    {
      ESP_LOGE(TAG, "write to nv flash error, err id is %d", err);
      res = -1;
      goto err_done;
    }
    err = nvs_commit(my_handle);
    if(err != ESP_OK)
    {
      ESP_LOGE(TAG, "commit failed");
      res = -1;
      goto err_done;
    }

    // Read check
    if(item == OTA_SET_VERSION || item == OTA_SET_ALL)
    {  
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      err = nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_VERSION, check_buffer, &len);
      if(strcmp(esp32_wifi_ota_config_structure.info_t.esp32_firmware_version, check_buffer) != 0)
      {
        ESP_LOGE(TAG, "version read from nv is not matched");
      }
    }
    if(item == OTA_SET_STA_SSID || item == OTA_SET_ALL)
    {  
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      err |= nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_STA_SSID, check_buffer, &len);
      if(strcmp(esp32_wifi_ota_config_structure.info_t.sta_ssid, check_buffer) != 0)
      {
        ESP_LOGE(TAG, "wifi_ssid read from nv is not matched");
      }
    }
    if(item == OTA_SET_STA_PASSWORD || item == OTA_SET_ALL)
    {  
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      err |= nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_STA_PASSWORD, check_buffer, &len);
      if(strcmp(esp32_wifi_ota_config_structure.info_t.sta_password, check_buffer) != 0)
      {
        ESP_LOGE(TAG, "wifi_password read from nv is not matched");
      }
    }
    if(item == OTA_SET_HOST || item == OTA_SET_ALL)
    {  
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      err |= nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_HOST, check_buffer, &len);
      if(strcmp(esp32_wifi_ota_config_structure.info_t.ota_information_file_host, check_buffer) != 0)
      {
        ESP_LOGE(TAG, "ota_info_host read from nv is not matched %s\n, %s", esp32_wifi_ota_config_structure.info_t.ota_information_file_host, check_buffer);
      }
    }
    if(item == OTA_SET_URL || item == OTA_SET_ALL)
    {  
      len = esp32_WIFI_OTA_URL_LENGTH_MAX;
      err |= nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_URL, check_buffer, &len);
      if(strcmp(esp32_wifi_ota_config_structure.info_t.ota_information_file_url, check_buffer) != 0)
      {
        ESP_LOGE(TAG, "ota_info_url read from nv is not matched %s\n, %s", esp32_wifi_ota_config_structure.info_t.ota_information_file_url, check_buffer);
      }
    }
    if(err != ESP_OK)
    {
      ESP_LOGE(TAG, "read from nv flash error, err id is %d, erase and retry times is %d", err, retry_time);
      nvs_erase_all(my_handle);
      nvs_close(my_handle);
      if(retry_time++ < ESP32_OTA_CONFIG_WRITE_RETRY_TIMES)
      {
        goto wtite_start;
      }
      else
      {
        ESP_LOGE(TAG, "NVS damaged, can't write config into NVS ");
        return -2;
      }
    }
    else
    {
      ESP_LOGE(TAG, "write successed");
      nvs_close(my_handle);
      return 0;
    }
  }
err_done:
  // Close
  nvs_close(my_handle);
  return res;
}

int esp32_ota_read_config_from_nv_flash(void)
{
  esp_err_t err;
  nvs_handle my_handle;
  err = nvs_open(ESP32_OTA_CONFIG_NV_NAMESPACE, NVS_READONLY, &my_handle);
  if(err != ESP_OK)
  {
    ESP_LOGE(TAG, "Error (%d) opening NVS handle!", err);
    return -1;
  } 
  else 
  {
    ESP_LOGI(TAG, "nv open successed");

    size_t len = esp32_WIFI_OTA_URL_LENGTH_MAX;
    err = nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_VERSION, esp32_wifi_ota_config_structure.info_t.esp32_firmware_version, &len);
    len = esp32_WIFI_OTA_URL_LENGTH_MAX;
    err = nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_STA_SSID, esp32_wifi_ota_config_structure.info_t.sta_ssid, &len);
    len = esp32_WIFI_OTA_URL_LENGTH_MAX;
    err = nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_STA_PASSWORD, esp32_wifi_ota_config_structure.info_t.sta_password, &len);
    len = esp32_WIFI_OTA_URL_LENGTH_MAX;
    err = nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_HOST, esp32_wifi_ota_config_structure.info_t.ota_information_file_host, &len);
    len = esp32_WIFI_OTA_URL_LENGTH_MAX;
    err = nvs_get_str(my_handle, ESP32_OTA_CONFIG_NV_URL, esp32_wifi_ota_config_structure.info_t.ota_information_file_url, &len);

    if(err != ESP_OK)
    {
      ESP_LOGE(TAG, "read from nv flash error, err id is %d", err);
      nvs_close(my_handle);
      return -1;
    }
    else
    {
      esp32_ota_config_show("read");
      nvs_close(my_handle);
      return 0;
    }
  }
}

void esp32_ota_start(void)
{
  esp_err_t err;
  /* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
  const esp_partition_t *update_partition = NULL;
  const esp_partition_t *configured = esp_ota_get_boot_partition();
  if(configured == NULL)
  {
    ESP_LOGE(TAG, "get next boot partition error");
    return;
  }
  const esp_partition_t *running = esp_ota_get_running_partition();
  
  if(configured != running)
  { 
    ESP_LOGE(TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x",
             configured->address, running->address);
    ESP_LOGE(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
  }
  ESP_LOGE(TAG, "Running partition type %d subtype %d (offset 0x%08x)",
           running->type, running->subtype, running->address);

  update_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, "factory");
  ESP_LOGE(TAG, "Writing to partition subtype %d at offset 0x%x",
           update_partition->subtype, update_partition->address);
  if(update_partition == NULL)
  {
    ESP_LOGE(TAG, "not found the update partition, can't do a OTA update");
    return;
  }
  err = esp_ota_set_boot_partition(update_partition);
  if(err != ESP_OK)
  {
    ESP_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%x", err);
  }
  else
  {
    ESP_LOGE(TAG, "Prepare to restart system!");
    esp_restart();
  }
}

