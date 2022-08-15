#ifndef __DRV_LTR381_H__
#define __DRV_LTR381_H__

#include "esp_err.h"
#include "driver/gpio.h"

/*******************************************************************************
 *********************************   MACROS   **********************************
 ******************************************************************************/
#define WHITE_LED_PIN                           (46)
#define DEV_POWER_ON    1
#define DEV_POWER_OFF   0

#define LTR381_ADDR_TRANS(n)                                ((n) << 1)
#define LTR381_GET_BITSLICE(uint8Val, bitName)              (((uint8Val) & (LTR381_##bitName##__MSK)) >> (LTR381_##bitName##__POS))
#define LTR381_SET_BITSLICE(uint8Val, bitName, bitVal)      (((uint8Val) & ~(LTR381_##bitName##__MSK)) | ((bitVal << (LTR381_##bitName##__POS)) & (LTR381_##bitName##__MSK)))

/*******************************************************************************
 **************************   SENSOR SPECIFICATIONS   **************************
 ******************************************************************************/
/* I2C device address */
#define LTR381_SLAVE_ADDR                       (0x53)
#define LTR381_I2C_ADDR                         LTR381_ADDR_TRANS(LTR381_SLAVE_ADDR)

/* Device info */
#define LTR381_PART_ID_VAL                      0xC2
#define LTR381_MANUFAC_ID_VAL                   0x05

#define LTR381_WAIT_TIME_PER_CHECK              (10)
#define LTR381_WAIT_TIME_TOTAL                  (100)

/*******************************************************************************
 *************   Non-Configurable (Unless data sheet is updated)   *************
 ******************************************************************************/
/* Device register set address */
#define LTR381_MAIN_CTRL_REG                    (0x00)
#define LTR381_ALS_CS_MEAS_RATE_REG             (0x04)
#define LTR381_ALS_CS_GAIN_REG                  (0x05)
#define LTR381_PART_ID_REG                      (0x06)
#define LTR381_MAIN_STATUS_REG                  (0x07)
#define LTR381_CS_DATA_IR_0_REG                 (0x0A)
#define LTR381_CS_DATA_IR_1_REG                 (0x0B)
#define LTR381_CS_DATA_IR_2_REG                 (0x0C)
#define LTR381_CS_DATA_GREEN_0_REG              (0x0D)
#define LTR381_CS_DATA_GREEN_1_REG              (0x0E)
#define LTR381_CS_DATA_GREEN_2_REG              (0x0F)
#define LTR381_CS_DATA_RED_0_REG                (0x10)
#define LTR381_CS_DATA_RED_1_REG                (0x11)
#define LTR381_CS_DATA_RED_2_REG                (0x12)
#define LTR381_CS_DATA_BLUE_0_REG               (0x13)
#define LTR381_CS_DATA_BLUE_1_REG               (0x14)
#define LTR381_CS_DATA_BLUE_2_REG               (0x15)
#define LTR381_INT_CFG_REG                      (0x19)
#define LTR381_INT_PST_REG                      (0x1A)
#define LTR381_ALS_THRES_UP_0_REG               (0x21)
#define LTR381_ALS_THRES_UP_1_REG               (0x22)
#define LTR381_ALS_THRES_UP_2_REG               (0x23)
#define LTR381_ALS_THRES_LOW_0_REG              (0x24)
#define LTR381_ALS_THRES_LOW_1_REG              (0x25)
#define LTR381_ALS_THRES_LOW_2_REG              (0x26)

/* Register MAIN_CTRL field */
#define LTR381_ALS_CS_ENABLE__REG               (LTR381_MAIN_CTRL_REG)
#define LTR381_ALS_CS_ENABLE__POS               (1)
#define LTR381_ALS_CS_ENABLE__MSK               (0x02)
#define LTR381_CS_MODE__REG                     (LTR381_MAIN_CTRL_REG)
#define LTR381_CS_MODE__POS                     (2)
#define LTR381_CS_MODE__MSK                     (4)
#define LTR381_SW_RESET__REG                    (LTR381_MAIN_CTRL_REG)
#define LTR381_SW_RESET__POS                    (4)
#define LTR381_SW_RESET__MSK                    (0x10)

/* Register ALS_CS_MEAS_RATE field */
#define LTR381_ALS_CS_MEAS_RATE__REG            (LTR381_ALS_CS_MEAS_RATE_REG)
#define LTR381_ALS_CS_MEAS_RATE__POS            (0)
#define LTR381_ALS_CS_MEAS_RATE__MSK            (0x07)
#define LTR381_ALS_CS_RESOLUTION__REG           (LTR381_ALS_CS_MEAS_RATE_REG)
#define LTR381_ALS_CS_RESOLUTION__POS           (4)
#define LTR381_ALS_CS_RESOLUTION__MSK           (0x70)

/* Register ALS_CS_GAIN field */
#define LTR381_ALS_CS_GAIN_RANGE__REG           (LTR381_ALS_CS_GAIN_REG)
#define LTR381_ALS_CS_GAIN_RANGE__POS           (0)
#define LTR381_ALS_CS_GAIN_RANGE__MSK           (0x07)

/* Register PART_ID field */
#define LTR381_PART_NUMBER_ID__REG              (LTR381_PART_ID_REG)
#define LTR381_PART_NUMBER_ID__POS              (4)
#define LTR381_PART_NUMBER_ID__MSK              (0xF0)

/* Register MAIN_STATUS field */
#define LTR381_ALS_CS_DATA_STATUS__REG          (LTR381_MAIN_STATUS_REG)
#define LTR381_ALS_CS_DATA_STATUS__POS          (3)
#define LTR381_ALS_CS_DATA_STATUS__MSK          (0x08)
#define LTR381_ALS_CS_INT_STATUS__REG           (LTR381_MAIN_STATUS_REG)
#define LTR381_ALS_CS_INT_STATUS__POS           (4)
#define LTR381_ALS_CS_INT_STATUS__MSK           (0x10)
#define LTR381_POWER_ON_STATUS__REG             (LTR381_MAIN_STATUS_REG)
#define LTR381_POWER_ON_STATUS__POS             (5)
#define LTR381_POWER_ON_STATUS__MSK             (0x20)

/* Register INT_CFG field */
#define LTR381_ALS_INT_PIN_EN__REG              (LTR381_INT_CFG_REG)
#define LTR381_ALS_INT_PIN_EN__POS              (2)
#define LTR381_ALS_INT_PIN_EN__MSK              (0x04)
#define LTR381_ALS_INT_SEL__REG                 (LTR381_INT_CFG_REG)
#define LTR381_ALS_INT_SEL__POS                 (4)
#define LTR381_ALS_INT_SEL__MSK                 (0x30)

/* Register INT_PST field */
#define LTR381_ALS_CS_PERSIST__REG              (LTR381_INT_PST_REG)
#define LTR381_ALS_CS_PERSIST__POS              (4)
#define LTR381_ALS_CS_PERSIST__MSK              (0xF0)

/* Field value enumeration */
typedef enum {
    LTR381_ALS_STANDBY_MODE = 0x00,
    LTR381_ALS_ACTIVE_MODE = 0x01,
} LTR381_ALS_CS_ENABLE_VAL;

typedef enum {
    LTR381_ALS_MODE = 0x00,
    LTR381_CS_MODE = 0x01,
} LTR381_ALS_CS_MODE_VAL;

typedef enum {
    LTR381_ALS_NO_RESET = 0x00,
    LTR381_ALS_RESET = 0x01,
} LTR381_SW_RESET_VAL;

typedef enum {
    LTR381_ALS_CS_MEAS_RATE_25MS = 0x00,
    LTR381_ALS_CS_MEAS_RATE_50MS = 0x01,
    LTR381_ALS_CS_MEAS_RATE_100MS = 0x02,
    LTR381_ALS_CS_MEAS_RATE_200MS = 0x03,
    LTR381_ALS_CS_MEAS_RATE_500MS = 0x04,
    LTR381_ALS_CS_MEAS_RATE_1000MS = 0x05,
    LTR381_ALS_CS_MEAS_RATE_2000MS = 0x06,
} LTR381_ALS_CS_MEAS_RATE_VAL;

typedef enum {
    LTR381_ALS_CS_RESOLUTION_20BIT = 0x00,
    LTR381_ALS_CS_RESOLUTION_19BIT = 0x01,
    LTR381_ALS_CS_RESOLUTION_18BIT = 0x02,
    LTR381_ALS_CS_RESOLUTION_17BIT = 0x03,
    LTR381_ALS_CS_RESOLUTION_16BIT = 0x04,
} LTR381_ALS_CS_RESOLUTION_VAL;

typedef enum {
    LTR381_ALS_CS_GAIN_RANGE_1 = 0x00,
    LTR381_ALS_CS_GAIN_RANGE_3 = 0x01,
    LTR381_ALS_CS_GAIN_RANGE_6 = 0x02,
    LTR381_ALS_CS_GAIN_RANGE_9 = 0x03,
    LTR381_ALS_CS_GAIN_RANGE_18 = 0x04,
} LTR381_ALS_CS_GAIN_RANGE_VAL;

typedef enum {
    LTR381_CS_ALS_DATA_OLD = 0x00,
    LTR381_CS_ALS_DATA_NEW = 0x01,
} LTR381_CS_ALS_DATA_STATUS_VAL;

typedef enum {
    LTR381_CS_ALS_INT_FALSE = 0x00,
    LTR381_CS_ALS_INT_TRUE = 0x01,
} LTR381_CS_ALS_INT_STATUS_VAL;

typedef enum {
    LTR381_POWER_ON_NORMAL = 0x00,
    LTR381_POWER_ON_GOING = 0x01,
} LTR381_POWER_ON_STATUS_VAL;

typedef enum {
    LTR381_ALS_INT_INACTIVE = 0x00,
    LTR381_ALS_INT_ACTIVE = 0x01,
} LTR381_ALS_INT_PIN_EN_VAL;

typedef enum {
    LTR381_ALS_INT_IR_CHANNEL = 0x00,
    LTR381_ALS_INT_GREEN_CHANNEL = 0x01,
    LTR381_ALS_INT_BLUE_CHANNEL = 0x02,
    LTR381_ALS_INT_RED_CHANNEL = 0x03,
} LTR381_ALS_INT_SELECT_VAL;


typedef struct
{
    int32_t data[3];
} rgb_data_t;

extern rgb_data_t rgb_value;

extern void ltr381_sensor_update(void);
extern void white_led_on(bool state);
extern esp_err_t ltr381_init(void);
extern int32_t get_red_value(void);
extern int32_t get_green_value(void);
extern int32_t get_blue_value(void);
extern bool is_ltr381_initialized(void);

#endif /* __DRV_LTR381_H__ */ 
