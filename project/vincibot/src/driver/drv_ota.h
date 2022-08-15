#ifndef _DRV_OTA_
#define _DRV_OTA_

#define ESP32_WIFI_OTA_CONFIG_STORE_IN_NV    (1) 

#define ESP32_WIFI_OTA_CONFIG_FILE_NAME      ("ota_config.bin")
#define esp32_WIFI_OTA_URL_LENGTH_MAX     (128)
#define esp32_WIFI_OTA_HOST_LENGTH_MAX    (64)

#define ESP32_OTA_DEFAULT_VERSION         ("NONE")
#define ESP32_OTA_DEFAULT_WIFI_SSID       ("NONE")
#define ESP32_OTA_DEFAULT_WIFI_PASSWORD   ("NONE")
#define ESP32_OTA_DEFAULT_WIFI_HOST       ("NONE")
#define ESP32_OTA_DEFAULT_WIFI_URL        ("NONE")
#define ESP32_OTA_DEFAULT_TARGET_VERSION  ("NONE")

#define ESP32_OTA_CONFIG_WRITE_RETRY_TIMES (3)

#define WIFI_SSID_LENTH_MAX               (32)
#define WIFI_PASSWORD_LENTH_MAX           (32)

typedef enum
{
  AP_SSID,
  AP_PASSWORD,
  STA_SSID,
  STA_PASSWORD,
  OTA_INFO_HOST,
  OTA_INFO_URL,
  OTA_BIN_HOST,
  OTA_BIN_URL,
}esp32_ota_config_wifi_item_t;

typedef union
{
  struct
  {
    uint16_t wifi_mode; 
    char esp32_firmware_version[32];
    char sta_ssid[WIFI_SSID_LENTH_MAX];
    char sta_password[WIFI_PASSWORD_LENTH_MAX];
    char ap_ssid[WIFI_SSID_LENTH_MAX];
    char ap_password[WIFI_PASSWORD_LENTH_MAX];
    char ota_information_file_host[esp32_WIFI_OTA_HOST_LENGTH_MAX];   
    char ota_information_file_url[esp32_WIFI_OTA_URL_LENGTH_MAX];
    char ota_update_source_file_host[esp32_WIFI_OTA_HOST_LENGTH_MAX]; 
    char ota_update_source_file_url[esp32_WIFI_OTA_URL_LENGTH_MAX];
  }info_t;
  uint8_t data[546]; // 32 + 2 + 32 * 4 + 64 * 2 + 128 * 2 
}esp32_wifi_ota_config_structure_t;

extern void esp32_ota_config_show(char *tag);

/**
* @brief 
*     - call this function when system satrt to read ota config or reset the config datas 
* @param 
*     - NONE
* @return
*     - NONE
*/
extern void esp32_ota_config_init(void);

/**
* @brief 
*     - call this function to set OTA wif mode, STA default
* @param 
*     - mode
*       -- 1: sta  2: ap 
* @return
*     - NONE
*/
extern void esp32_ota_config_set_wifi_mode(uint8_t mode);

/**
* @brief 
*     - call this function to get ota wifi mode 
* @param 
*     - mode: point of mode out
*       -- 1: sta  2: ap 
* @return
*     - NONE
*/
extern void esp32_ota_config_get_wifi_mode(uint8_t *mode);

/**
* @brief 
*     - call this function to set wifi ssid or password
* @param 
*     - item:
*       -- one of esp32_ota_config_wifi_item_t
*     - str
*       -- ssid or password string
* @return
*     - NONE
*/
extern void esp32_ota_config_set_wifi_info(uint8_t item, const char * str);

/**
* @brief 
*     - call this function to get wifi ssid or password
* @param 
*     - item:
*       -- see esp32_ota_config_wifi_item_t
*     - str
*       -- ssid or password string out
* @return
*     - NONE
*/
extern void esp32_ota_config_get_wifi_info(uint8_t item, char *const str);

/**
* @brief 
*     - call this function to check if the namespace --ESP32_OTA_CONFIG_NV_NAMESPACE in NVS exist
* @param 
*     - None
* @return
*     - 0 : exist
*     - -1: not exist
*/
extern int esp32_ota_check_config_in_nv_flash(void);

/**
* @brief 
*     - this function will write the struct esp32_wifi_ota_config_structure that defined in esp32_ota_config.c
*     - to NVS
* @param 
*     - item: the specific item of config infomation
*       -- esp32_ota_config_wifi_item_t
* @return
*     - 0  write successed
*     - -1 write failed
*     - -2 NVS damaged, OTA update can't run
*/
extern int esp32_ota_write_config_info_to_nv_flash(uint8_t item);

/**
* @brief 
*     - this function will read the data in NVS into 
*     - esp32_wifi_ota_config_structure that defined in esp32_ota_config.c 
* @param 
*     - item: the specific item of config infomation
*       -- esp32_ota_config_wifi_item_t
* @return
*     - 0  read successed
*     - -1 read failed
*/
extern int esp32_ota_read_config_from_nv_flash(void);

/**
* @brief 
*     - this function will restart the system, and running the OTA update codes 
*     - when system launched next time
* @param 
*     - NONE
* @return
*     - NONE
*/
extern void esp32_ota_start(void);
	
#endif
