#ifndef _MT_LOG_H_
#define _MT_LOG_H_

#include "esp_log.h"

#define MT_LOG_NONE     0
#define MT_LOG_ERROR    1
#define MT_LOG_WARNING  2
#define MT_LOG_INFO     4
#define MT_LOG_DEBUG    5
#define MT_LOG_VERBOSE  6

#define MT_LOG_LEVEL ESP_LOG_NONE
#define MT_LOGE( tag, format, ... )  if (MT_LOG_LEVEL >= ESP_LOG_ERROR)   { esp_log_write(ESP_LOG_ERROR,   tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define MT_LOGW( tag, format, ... )  if (MT_LOG_LEVEL >= ESP_LOG_WARN)    { esp_log_write(ESP_LOG_WARN,    tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define MT_LOGI( tag, format, ... )  if (MT_LOG_LEVEL >= ESP_LOG_INFO)    { esp_log_write(ESP_LOG_INFO,    tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define MT_LOGD( tag, format, ... )  if (MT_LOG_LEVEL >= ESP_LOG_DEBUG)   { esp_log_write(ESP_LOG_DEBUG,   tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define MT_LOGV( tag, format, ... )  if (MT_LOG_LEVEL >= ESP_LOG_VERBOSE) { esp_log_write(ESP_LOG_VERBOSE, tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }


#endif /* _MT_LOG_H_ */
