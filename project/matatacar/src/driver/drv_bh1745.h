#ifndef __DRV_BH1745_H__
#define __DRV_BH1745_H__

#include "esp_err.h"
#include "driver/gpio.h"

/* Exported macro ------------------------------------------------------------*/
#define I2C_ERROR                       (-1)
#define COLORSENSOR_DEFAULT_ADDRESS     (0x38)//0011 1000

/* Register List define------------------------------------------------------------*/
#define SYSTEM_CONTROL         (0X40)
#define MODE_CONTROL1          (0X41)
#define MODE_CONTROL2          (0X42)
#define MODE_CONTROL3          (0X44)
#define RED_DATA_LSBs          (0X50)
#define RED_DATA_MSBs          (0X51)
#define GREEN_DATA_LSBs        (0X52)
#define GREEN_DATA_MSBs        (0X53)
#define BLUE_DATA_LSBs         (0X54)
#define BLUE_DATA_MSBs         (0X55)
#define CLEAR_DATA_LSBs        (0X56)
#define CLEAR_DATA_MSBs        (0X57)
#define DINT_DATA_LSBs         (0X58)
#define DINT_DATA_MSBs         (0X59)
#define INTERRUPT_REG          (0X60)
#define PERSISTENCE            (0X61)
#define TH_LSBs                (0X62)
#define TH_MSBs                (0X63)
#define TL_LSBs                (0X64)
#define TL_MSBs                (0X65)
#define MANUFACTURER_ID        (0X92)
#define CHIP_ID                (0XE0)

#define SW_RESET               (1 << 7)
#define INT_RESET              (1 << 6)

/* REG_MODECONTROL1(0x41) */
#define MEASURE_160MS          (0x00)
#define MEASURE_320MS          (0x01)
#define MEASURE_640MS          (0x02)
#define MEASURE_1280MS         (0x03)
#define MEASURE_2560MS         (0x04)
#define MEASUREMENT_MAX        (0x05)

extern void bh1745_sensor_update(void);
extern esp_err_t bh1745_init(void);
extern bool is_bh1745_initialized(void);

#endif /* __DRV_BH1745_H__ */ 
