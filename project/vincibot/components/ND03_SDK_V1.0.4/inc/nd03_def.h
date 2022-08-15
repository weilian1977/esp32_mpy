/**
 * @file nd03_def.h
 * @author tongsheng.tang
 * @brief ND03's Macro definition and data structure
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND03__DEF__H__
#define __ND03__DEF__H__

#include <stdint.h>
#include <stdio.h>

#define DEBUG_INFO        1 /*!< 调试信息打印开关 */
#define NX_PRINTF(fmt, args...)   do{if(DEBUG_INFO) printf(fmt, ##args); }while(0) /*!< 调试接口，默认使用printf */
#define VERSION_THRESHOLD 2 /*!< 版本兼容过滤 */


/** @defgroup ND03_Global_Define_Group ND03 Defines
 *	@brief	  ND03 Defines
 *	@{
 */


/** @defgroup ND03_Reg_Group ND03 Register Defines
 * @brief	  ND03 Register Defines
 *  @{
 */

#define ND03_REG_BOOT1                    0x55AA    /*!< ND03模组第一段启动寄存器，需要对该寄存器写1byte，且值为0x38 */
#define ND03_REG_BOOT2                    0x3BFC    /*!< ND03模组第二段启动寄存器，需要对该寄存器写4bytes，且值为0xFFFFFFF2 */
#define ND03_REG_BOOT3                    0x0888    /*!< ND03模组第三段启动寄存器，需要对该寄存器写4bytes，且值为0x00000001 */
#define ND03_REG_BOOT4                    0x3A1C    /*!< ND03模组第四段启动寄存器，需要对该寄存器写4bytes，且值为0x00000030 */
#define ND03_REG_BOOT5                    0x0808    /*!< ND03模组第五段启动寄存器，需要对该寄存器写4bytes，且值为0x00000001 */
#define ND03_REG_DEV_ID                   0x3B80    /*!< ND03模组设备ID */

#define ND03_REG_ADDR_BASE                (0xA700U)

/* Read Only(RO) */
#define ND03_REG_FW_VERSION               (ND03_REG_ADDR_BASE+0x00A0)  /*!< ND03模组固件版本 */
#define ND03_REG_STATE                    (ND03_REG_ADDR_BASE+0x00A4)  /*!< ND03模组运行状态 */
#define ND03_REG_AMP_DEPTH                (ND03_REG_ADDR_BASE+0x00A8)  /*!< 幅度值和深度值寄存器，[15:0]->uint16_t:深度值(mm), [31:16]->uint16_t:幅度值 */
#define ND03_REG_EXP_THERM                (ND03_REG_ADDR_BASE+0x00AC)  /*!< 积分时间和温度值寄存器, [15:0]->int16_t:温度值(0.1), [31:16]->uint16_t:积分时间(us)*/
#define ND03_REG_ERROR_FLAG               (ND03_REG_ADDR_BASE+0x00B0)  /*!< 错误返回寄存器 */


/* Read and Write(RW) */
#define ND03_REG_CALIB_OFFSET             (ND03_REG_ADDR_BASE+0x00BC)  /*!< 标定和串扰标定的实际深度寄存器， [15:0]->uint16_t:标定深度(mm), [31:16]->uint16_t:串扰标定深度(mm)*/
#define ND03_REG_RANGE_MODE               (ND03_REG_ADDR_BASE+0x00C0)  /*!< 测距模式配置寄存器 */
#define ND03_REG_SLAVE_ADDR               (ND03_REG_ADDR_BASE+0x00C4)  /*!< 模组IIC设备地址寄存器 */
#define ND03_REG_DATA_VAL_REQ             (ND03_REG_ADDR_BASE+0x00C8)  /*!< 数据请求和数据有效寄存器, [15:0]->uint16_t:数据请求, [31:16]->uint16_t:数据有效*/
#define ND03_REG_TOF_FLAG                 (ND03_REG_ADDR_BASE+0x00CC)  /*!< TOF Flag寄存器 */
#define ND03_REG_MAX_EXPOSURE             (ND03_REG_ADDR_BASE+0x00D0)  /*!< 最大积分时间寄存器 */
#define ND03_REG_FLASH_ADDR               (ND03_REG_ADDR_BASE+0x00D4)  /*!< 读取flash数据的地址寄存器 */
#define ND03_REG_FLASH_DATA               (ND03_REG_ADDR_BASE+0x00D8)  /*!< 读取flash的数据寄存器 */
#define ND03_REG_GPIO_DEPTH_TH            (ND03_REG_ADDR_BASE+0x00DC)  /*!< GPIO功能远近距离阈值寄存器, [15:0]->uint16_t:近距离阈值, [31:16]->uint16_t:远距离阈值 */
#define ND03_REG_GPIO_SETTING             (ND03_REG_ADDR_BASE+0x00E0)  /*!< GPIO中断输出功能配置寄存器 */
#define ND03_REG_SETTING                  (ND03_REG_ADDR_BASE+0x00E4)  /*!< 其他功能配置寄存器 */

/** @} ND03_Reg_Group */



/** @defgroup ND03_Boot_Group ND03 Boot Value
 *	@brief	  ND03 Boot  (ND03_REG_BOOT)
 *	@{
 */

#define ND03_REG_BOOT1_VALUE              0x38          /*!< ND03模组第一段启动值，且值为0x38 */
#define ND03_REG_BOOT2_VALUE              0xFFFFFFF2    /*!< ND03模组第二段启动值，且值为0xFFFFFFF2 */
#define ND03_REG_BOOT3_VALUE              0x00000001    /*!< ND03模组第三段启动值，且值为0x00000001 */
#define ND03_REG_BOOT4_VALUE              0x00000030    /*!< ND03模组第四段启动值，且值为0x00000030 */
#define ND03_REG_BOOT5_VALUE              0x00000001    /*!< ND03模组第五段启动值，且值为0x00000001 */
#define ND03_REG_DEV_ID_VALUE             0x002F0012    /*!< ND03模组设备ID默认值 */
/** @} ND03_Boot_Group */


/** @defgroup ND03_State_Group ND03 Data Request Index
 *	@brief	  ND03 State  (ND03_REG_STATE)
 *	@{
 */

#define ND03_STATE_IDLE                   0x00000000
#define ND03_STATE_SOFTWARE_READY         0x000000A5
#define ND03_STATE_GOT_DEPTH              0x000000A6

/** @} ND03_State_Group */

/** @defgroup ND03_Data_Val_Req_Idx_Group ND03 Data Request Index
 *	@brief	  ND03 Data Request Mask (ND03_REG_DATA_VAL_REQ)
 *	@{
 */
// REG_DATA_REQ MASK
#define ND03_DEPTH_DATA_REQ_MASK          0x00000001  /*!< 深度数据读取请求掩码 */
#define ND03_CALIBRATION_REQ_MASK         0x00000002  /*!< 非线性标定请求掩码 */
#define ND03_XTALK_CALIB_REQ_MASK         0x00000004  /*!< 串扰标定请求掩码 */
#define ND03_FLASH_DATA_REQ_MASK          0x00000100  /*!< flash数据读取请求掩码 */
// REG_DATA_VAL MASK
#define ND03_DEPTH_DATA_VAL_MASK          0x00010000  /*!< 深度数据有效位掩码 */
#define ND03_CALIBRATION_VAL_MASK         0x00020000  /*!< 非线性标定完成有效位掩码 */
#define ND03_XTALK_CALIB_VAL_MASK         0x00040000  /*!< 串扰标定完成有效位掩码 */
#define ND03_FLASH_DATA_VAL_MASK          0x01000000  /*!< flash数据有效位掩码 */

/** @} ND03_Data_Val_Req_Idx_Group */



/** @defgroup ND03_Tof_Flag_Mask_Group ND03 Setting Mask 
 *	@brief	  ND03 Tof Flag Mask (ND03_REG_TOF_FLAG)
 *	@{
 */

// REG_TOF_FLAG Mask
#define ND03_THERM_TOF_FLAG               0x00000001  /*!< 温度补偿配置掩码 */
#define ND03_LUT_TOF_FLAG                 0x00000100  /*!< 非线性矫正配置掩码 */
#define ND03_XT_TOF_FLAG                  0x00010000  /*!< 串扰矫正配置掩码 */
#define ND03_AE_TOF_FLAG                  0x01000000  /*!< 自动曝光配置掩码 */

/** @} ND03_Tof_Flag_Mask_Group */



/** @defgroup ND03_Gpio_Func_Group ND03 Gpio Functions Define
 *	@brief	  ND03 Gpio Functions Define Group (ND03_REG_GPIO_SETTING)
 *	@{
 */

// REG_INT_SETTING Mask
#define GPIO_FUNCTIONALITY                      0x00000007  /*!< GPIO功能配置掩码 */
#define GPIO_POLARITY_FLAG                      0x00000008  /*!< 极性配置掩码 */


// GPIO Functionality
typedef uint32_t  ND03_GpioFunc_t;
#define ND03_GPIO_FUNCTIONALITY_OFF                     ((ND03_GpioFunc_t) 0x00000000) /*!< No Interrupt */
#define ND03_GPIOFUNCTIONALITY_THRESHOLD_LOW            ((ND03_GpioFunc_t) 0x00000001) /*!< Level Low (value < REG_GPIO_DEPTH_LOW_TH) */
#define ND03_GPIOFUNCTIONALITY_THRESHOLD_HIGH           ((ND03_GpioFunc_t) 0x00000002) /*!< Level High (value > REG_GPIO_DEPTH_HIGH_TH) */
#define ND03_GPIOFUNCTIONALITY_THRESHOLD_DOMAIN_OUT     ((ND03_GpioFunc_t) 0x00000003) /*!< Out of domain (value < REG_GPIO_DEPTH_LOW_TH OR value > REG_GPIO_DEPTH_HIGH_TH) */
#define ND03_GPIOFUNCTIONALITY_NEW_MEASURE_READY        ((ND03_GpioFunc_t) 0x00000004) /*!< New Depth Ready */

// GPIO polarity
typedef uint32_t ND03_GpioPolar_t;
#define ND03_GPIO_POLARITY_LOW                          ((ND03_GpioPolar_t) 0x00000000) /*!< Negative polarity, Low level valid*/
#define ND03_GPIO_POLARITY_HIGH                         ((ND03_GpioPolar_t) 0x00000001) /*!< Positive polarity, High level valid*/

/** @} ND03_Gpio_Func_Group */


/** @defgroup ND03_Setting_Group ND03 Setting Group
 *  @brief	  ND03 Setting Group (ND03_REG_SETTING)
 *	@{
 */
#define ND03_SETTING_SW_SLEEP_MASK          0x00000001
/** @} ND03_Setting_Group */


/** @defgroup ND03_Error_Group ND03 Error Group
 *  @brief	  ND03 Error Group (ND03_REG_ERROR_FLAG)
 *	@{
 */
#define ND03_ERROR_NONE                      0  /*!< 成功 */
#define ND03_ERROR_CALIBRATION              -1  /*!< 标定失败错误 */
#define ND03_ERROR_TIME_OUT                 -2  /*!< 超时错误 */
#define ND03_ERROR_I2C			                -3  /*!< IIC通讯错误 */
#define ND03_ERROR_AMP			                -4  /*!< 幅度值错误 */
#define ND03_ERROR_FW                       -5  /*!< 固件版本兼容错误 */
#define ND03_ERROR_BOOT                     -6  /*!< 模组启动错误 */
#define ND03_ERROR_HOST_ENDIAN              -7  /*!< 主机大小端配置错误 */
/** @} ND03_Error_Group */


/** @defgroup ND03_Device_Mode_Group ND03 Device Mode
 *  @brief  ND03 Device Mode (ND03_REG_RANGE_MODE)
 *	@{
 */

#define ND03_RANGE_MODE_MASK                      ((uint32_t)0x00000001)
#define ND03_DEVICEMODE_MASK                      ((uint32_t)0x00000002)
#define ND03_PIXEL_NUM_MODE_MASK                  ((uint32_t)0x00000008)
#define ND03_WORK_SCENE_MASK                      ((uint32_t)0x00000020)

#define ND03_RANGE_MODE_NORMAL                    ((uint32_t)0x00000000)              /*!< 普通模式 */
#define ND03_RANGE_MODE_FAST                      ((uint32_t)0x00000001)              /*!< 快速模式 */

#define ND03_DEVICEMODE_SINGLE_RANGING            ((uint32_t)0x00000000)              /*!< 单次测量 */
#define ND03_DEVICEMODE_CONTINUOUS_RANGING        ((uint32_t)0x00000002)              /*!< 连续测量 */

#define ND03_PIXEL_NUM_MODE_SPOT                  ((uint32_t)0x00000000)               /*!< 单个像素点 */ 
#define ND03_PIXEL_NUM_MODE_ALL                   ((uint32_t)0x00000008)               /*!< 全部像素 */

#define ND03_WORK_SCENE_INDOOR                    ((uint32_t)0x00000000)              /*!< 室内模式 */
#define ND03_WORK_SCENE_OUTDOOR                   ((uint32_t)0x00000020)              /*!< 室外模式 */

/** @} ND03_Device_Mode_Group */



/** @defgroup ND03_Slave_Address_Group ND03 Device Mode
 *  @brief  ND03 Slave Address (ND03_REG_Slave_ADDR)
 *	@{
 */

#define ND03_DEFAULT_SLAVE_ADDR             0x5B  /*!< ND03默认IIC地址(7bit) */

/** @} ND03_Global_Define_Group */



/** @} ND03_Global_Define_Group */

/** @enum  ND03_Status_e
 *  @brief 定义ND03状态宏
 */
typedef enum{
    ND03_DISABLE = 0,   ///< 关闭状态
    ND03_ENABLE  = 1    ///< 使能状态
  } ND03_Status_e;

/** @enum  ND03_GPIOPolarity_e
 *  @brief 定义模组GPIO输出极性
 */
typedef enum{
    ND03_GPIO_POLAR_LOW   = 0,    ///< 低电平有效
    ND03_GPIO_POLAR_HIGH  = 1,    ///< 高电平有效
}ND03_GPIOPolarity_e;


/** 
  * @struct ND03_RangingData_t
  * 
  * @brief ND03测量结果结构体 \n
  * 定义存储ND03的深度、幅度信息
  */
typedef struct{
    uint16_t depth; ///< 测量距离/mm
    uint16_t amp;   ///< 测量幅度值，作为信号可信度
}ND03_RangingData_t;


/** 
  * @struct ND03_ChipInfo_t
  * 
  * @brief ND03模组生产信息\n
  */
 typedef struct {
    uint32_t  nd03_fw_version;        ///< ND03固件版本
    uint32_t  nd03_product_date;      ///< ND03生产日期
    uint32_t  nd03_product_time;      ///< ND03生产时间
    uint32_t  nd03_model_revision;    ///< ND03设备类型及硬件版本
    uint8_t   nd03_chip_id[32];       ///< ND03唯一芯片ID,字符串
 } ND03_ChipInfo_t;

/** 
  * @struct ND03_DevConfig_t
  * 
  * @brief ND03模组配置数据\n
  */
typedef struct {
    uint32_t        range_mode;             ///< 模组取图模式配置
} ND03_DevConfig_t;

/** 
  * @struct ND03_Dev_t
  * 
  * @brief 设备类型结构体\n
  */
typedef struct {
    void(*SetXShutPinLevelFunc)(uint32_t );   ///< 如果设置XShut电平函数指针，参数值为0，则输出低电平，否则输出高电平
    ND03_DevConfig_t  config;                 ///< 模组配置信息
    ND03_ChipInfo_t   chip_info;              ///< 模组生产信息
    uint8_t           i2c_dev_addr;           ///< i2c设备地址
    uint32_t          dev_pwr_state;          ///< 设备的当前状态, 就绪模式或者睡眠模式
} ND03_Dev_t;


/** 
 * @struct ND03_Func_Ptr_t
 * 
 * @brief 链接平台的API\n
 */
typedef struct{
	int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t); ///< i2c写函数
	int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t); ///< i2c读函数
	void(*Delay1msFunc)(uint32_t );   ///< 延时1ms函数
	void(*Delay10usFunc)(uint32_t );  ///< 延时10us函数
}ND03_Func_Ptr_t;

#endif

