#ifndef _FIRMWARE_VERSION_H_
#define _FIRMWARE_VERSION_H_

#define FIRMWARE_VERSION_LENGTH_MAX   (32)

#define SOFT_VERSION  "00.00.07"

//#define HARDWARE_VERSION_CHECK_CHANNEL      (ADC1_CHANNEL_1)        //GPIO2

#define HARDWARE_VERSION_NUM0     0.0f
#define HARDWARE_VERSION_NUM1     0.206f
#define HARDWARE_VERSION_NUM2     0.416f
#define HARDWARE_VERSION_NUM3     0.624f
#define HARDWARE_VERSION_NUM4     0.825f
#define HARDWARE_VERSION_NUM5     1.031f
#define HARDWARE_VERSION_NUM6     1.222f
#define HARDWARE_VERSION_NUM7     1.433f
#define HARDWARE_VERSION_NUM8     1.65f
#define HARDWARE_VERSION_NUM9     1.867f
#define HARDWARE_VERSION_NUM10    2.078f
#define HARDWARE_VERSION_NUM11    2.268f
#define HARDWARE_VERSION_NUM12    2.475f


#define HARDWARE_VERSION_OFFSET   0.08f

/******************************************************************************
 DELCARE PUBLIC FUNCTIONS
 ******************************************************************************/
extern const char *get_firmware_version_t(void);
// extern void hardware_version_check_init(void);
// extern void hardware_version_voltage_update(void);
// extern uint8_t get_hardware_version_t(void);
extern int firmware_version_append_t(char *str);
extern void show_firmware_version_t(void);

#endif /* _FIREFLY_FIRMWARE_VERSION_H_ */


