
#ifndef __DRV_NVS_H__
#define __DRV_NVS_H__

#define NVS_STRING_LENGTH_MAX  (128)

esp_err_t nvs_init(void);
esp_err_t nvs_write_string(const char* namespace, const char* key, char* write_string);
esp_err_t nvs_read_string(const char* namespace, const char* key, char* read_string);

#endif
