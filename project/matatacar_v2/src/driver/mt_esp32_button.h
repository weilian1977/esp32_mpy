#ifndef _MT_ESP32_BUTTON_H_
#define _MT_ESP32_BUTTON_H_

#include "mt_err.h"

mt_err_t mt_esp32_button_init_t(void);
mt_err_t mt_esp32_button_test_t(void);
mt_err_t mt_esp32_button_update_t(void);
mt_err_t mt_esp32_button_event_listening(uint8_t cur_value);


mt_err_t mt_esp32_button_get_level_t(uint8_t button_id, uint8_t *level);
mt_err_t mt_esp32_button_is_pressed_t(uint8_t button_id, bool *sta);
mt_err_t mt_esp32_button_is_released_t(uint8_t button_id, bool *sta);

#endif /* _MT_ESP32_BUTTON_H_ */
