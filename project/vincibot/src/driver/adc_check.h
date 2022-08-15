#ifndef __ADC_CHECK_H__
#define __ADC_CHECK_H__

#include "esp_err.h"

#define DEFAULT_VREF                        (1100) 
#define PLAY_KEY_AND_USB_DETECT_CHANNEL      (ADC1_CHANNEL_1)        //GPIO2
#define AB_KEY_DETECT_CHANNEL                (ADC1_CHANNEL_5)        //GPIO6

#define ADC_DETECT_OFFSET                0.15f
#define PLAY_KEY_PRESSED                 1.56f
#define USB_INSERT                       1.88f
#define USB_INSERT_AND_PLAY_KEY_PRESSED  1.12f
#define A_KEY_PRESSED                    1.56f
#define B_KEY_PRESSED                    1.88f
#define A_KEY_PRESSED_AND_B_KEY_PRESSED  1.12f
extern void adc_check_init(void);
extern void adc_check_update(void);
extern float get_play_key_and_usb_detect_voltage(void);
extern float get_ab_key_detect_voltage(void);
#endif /* __ADC_CHECK_H__ */ 
