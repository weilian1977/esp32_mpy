#ifndef __USB_DETECT_H__
#define __USB_DETECT_H__

#include "esp_err.h"

#define USB_DETECT_PIN     47

extern void usb_pin_init(void);
extern void usb_status_update(void);
extern bool is_usb_detected(void);

#endif /* __USB_DETECT_H__ */ 
