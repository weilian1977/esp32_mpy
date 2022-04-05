#ifndef __INDICATOR_LED_H__
#define __INDICATOR_LED_H__

#include "esp_err.h"

#define LED_SINGLE_FLASH_PERIODIC_TIME                  420
#define LED_SINGLE_FAST_FLASH_PERIODIC_TIME             140
#define LED_DOUBLE_FLASH_PERIODIC_TIME                  35
#define LED_DOUBLE_FLASH_GAP_TIME                       (LED_DOUBLE_FLASH_PERIODIC_TIME * 20)
#define INDICATOR_LED_PIN                               (21)

typedef enum name
{
    LED_BLUE_OFF                       = 0x00,
    LED_BLUE_ON                        = 0x01,
    LED_BLUE_SINGLE_FLASH              = 0x02,
    LED_BLUE_DOUBLE_FLASH              = 0x03,
    LED_BLUE_SINGLE_FAST_FLASH         = 0x04,
}indicator_led_state_t;

extern void indicator_led_init(void);

//extern void indicator_led_set(indicator_led_state_t led_state);
void indicator_led_set(uint32_t led_state);

extern void indicator_led_update(void);

#endif /* __INDICATOR_LED_H__ */ 
