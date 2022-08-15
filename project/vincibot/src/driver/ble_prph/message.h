#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"


#include "esp_err.h"

#define MSG_CRC_LEN              (2)
#define MSG_TOTAL_LEN_MIN        (5)

#define MSG_HEAD_LEN                      (0x02)
#define MSG_FRAMER_HEADER                 (0XFE)
#define MSG_FRAMER_TRANSLATION            (0xFD)
#define CHAR_TRANSLATION_OFFSET           (0x20)

#define MESSAGE_RX_OK                           (0)
#define MESSAGE_ERROR_CRC                       (1)
#define MESSAGE_ERROR_BUFFER_OVERFLOW           (2)
#define MESSAGE_ERROR_NOT_SUPPORTED_CMD         (3)
#define MESSAGE_ERROR_INVALID_PARAM             (4)
#define MESSAGE_ERROR_INVALID_LENGTH            (5)
#define MESSAGE_ERROR_TIMEOUT                   (6)
#define MESSAGE_ERROR_CONTROL_MODE_ERROR        (7)

typedef enum{
    CMD_ASSIC_FUNCTION = 0x70,
    CMD_SET_NEW_PROTOCOL = 0x7e,
    CMD_CANCEL = 0x84,
    CMD_GENERAL_RSP = 0x88,
}COMMEND_TYPE_E;

#endif

