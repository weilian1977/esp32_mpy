/**
 * @file nd03_dev.h
 * @author tongsheng.tang
 * @brief ND03 device setting functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND03_DEV_H__
#define __ND03_DEV_H__

#include "nd03_def.h"

/** @defgroup ND03_Dev_Group ND03 Device Funtions
 *  @brief ND03 Device Funtions
 *  @{
 */

/**
 * @brief 获取当前SDK的软件版本号
 * @return  uint32_t   
 * @retval  软件版本号
 */
uint32_t ND03_GetSdkVersion(void);


/**
 * @brief 初始化ND03库的API
 * @param   dev_op   ND03库的API入口地址
 * @return  void   
 */
void ND03_RegisteredPlatformApi(ND03_Func_Ptr_t dev_op);


/*! 
 * @brief 设置测量模式
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param mode: 模式数据
 * @return int32_t
*/
int32_t ND03_SetRangeMode(ND03_Dev_t *pNxDevice, uint32_t mode);


/**
 * @brief 获取当前测量模式
 *
 * @param   pNxDevice 模组设备
 * @return  uint32_t 测量模式   
 * - ::ND03_DEVICEMODE_SINGLE_RANGING:单次测量
 * - ::ND03_DEVICEMODE_CONTINUOUS_RANGING:连续测量
 * - ::ND03_RANGE_MODE_FAST:快速模式
 * - ::ND03_RANGE_MODE_NORMAL:普通模式
 */
uint32_t ND03_GetRangeMode(ND03_Dev_t *pNxDevice);


/*! 
 * @brief 设置ND03的设备地址
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 配置ND03模组的i2c设备地址
 * @return int32_t
*/
int32_t ND03_SetSlaveAddr(ND03_Dev_t *pNxDevice, uint8_t addr);


/*! 
 * @brief 获取ND03的设备地址
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 获取的ND03模组i2c设备地址
 * @return int32_t
*/
int32_t ND03_GetSlaveAddr(ND03_Dev_t *pNxDevice, uint8_t *addr);


/*!
 * @brief ND03 Set Lut State to Register
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: ND03_ENABLE or ND03_DISABLE
 * @return int32_t
*/
int32_t ND03_SetLutState(ND03_Dev_t *pNxDevice, ND03_Status_e flag);


/*!
 * @brief ND03 Get Lut State From Register
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: 获取到的Lut状态变量指针
 * @return int32_t
*/
int32_t ND03_GetLutState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag);


/*!
 * @brief ND03 Set Auto Exposure State to Register
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: ND03_ENABLE or ND03_DISABLE
 * @return int32_t
*/
int32_t ND03_SetAutoExpState(ND03_Dev_t *pNxDevice, ND03_Status_e flag);


/*!
 * @brief ND03 Get Auto Exposure State From Register
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: 获取到的ND03自动曝光状态变量指针
 * @return int32_t
*/
int32_t ND03_GetAutoExpState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag);


/*!
 * @brief ND03 Set Cross Talk State to Register
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: ND03_ENABLE or ND03_DISABLE
 * @return int32_t
*/
int32_t ND03_SetXTalkState(ND03_Dev_t *pNxDevice, ND03_Status_e flag);


/*!
 * @brief ND03 Get Cross Talk State From Register
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: 获取到的ND03串扰矫正状态变量指针
 * @return int32_t
*/
int32_t ND03_GetXTalkState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag);


/*!
 * @brief ND03 Set Therm Compensation State to Register
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: ND03_ENABLE or ND03_DISABLE
 * @return int32_t
*/
int32_t ND03_SetThermCompensationState(ND03_Dev_t *pNxDevice, ND03_Status_e flag);


/*!
 * @brief ND03 Get Therm Compensation State From Register
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: 获取到的ND03温度矫正状态变量指针
 * @return int32_t
*/
int32_t ND03_GetThermCompensationState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag);


/*!
 * @brief ND03 GPIO Config 
 * @param pNxDevice: ND03模组设备信息结构体
 * @param functionality: ND03_GPIO_FUNCTIONALITY_OFF
 *                       ND03_GPIOFUNCTIONALITY_THRESHOLD_LOW 
 *                       ND03_GPIOFUNCTIONALITY_THRESHOLD_HIGH
 *                       ND03_GPIOFUNCTIONALITY_THRESHOLD_DOMAIN_OUT
 *                       ND03_GPIOFUNCTIONALITY_NEW_MEASURE_READY
 * @param polarity: 数据有效时，GPIO的电平(0 or 1)
 * @return int32_t
*/
int32_t ND03_SetGpioConfig(ND03_Dev_t *pNxDevice, ND03_GpioFunc_t functionality, uint32_t polarity);


/*! 
 * @brief ND03 Get Gpio Config
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param functionality: 获取到的gpio功能变量指针
 * @param polarity: 获取到的gpio极性变量指针
 * @return int32_t
*/
int32_t ND03_GetGpioConfig(ND03_Dev_t *pNxDevice, ND03_GpioFunc_t *functionality, uint32_t *polarity);


/*!
 * @brief ND03 Set Depth Threshold (设置深度阈值)
 * @param pNxDevice: ND03模组设备信息结构体
 * @param depth_low:   低深度阈值 / mm
 * @param depth_high:  高深度阈值 / mm
 * @return int32_t
*/
int32_t ND03_SetDepthThreshold(ND03_Dev_t *pNxDevice, uint16_t depth_low, uint16_t depth_high);


/*!
 * @brief ND03 Get Depth Threshold (获取深度阈值)
 * @param pNxDevice: ND03模组设备信息结构体
 * @param depth_low:   低深度阈值 / mm
 * @param depth_high:  高深度阈值 / mm
 * @return int32_t
*/
int32_t ND03_GetDepthThreshold(ND03_Dev_t *pNxDevice, uint16_t *depth_low, uint16_t *depth_high);



/*!
 * @brief ND03 Set Maximum Exposure (获取高深度阈值)
 * @param pNxDevice: ND03模组设备信息结构体
 * @param max_exposure_us:  最大积分时间 / us
 * @return int32_t
*/
int32_t ND03_SetMaxExposure(ND03_Dev_t *pNxDevice, uint16_t max_exposure_us);


/*!
 * @brief ND03 Get Maximum Exposure (获取高深度阈值)
 * @param pNxDevice: ND03模组设备信息结构体
 * @param max_exposure_us:  获取的最大积分时间指针 / us
 * @return int32_t
*/
int32_t ND03_GetMaxExposure(ND03_Dev_t *pNxDevice, uint16_t *max_exposure_us);


/**
 * @brief 获取当前ND03的积分时间
 * @param   pNxDevice   模组设备
 * @return  uint32_t    当前积分时间/us
 */
uint16_t ND03_GetCurrentExp(ND03_Dev_t *pNxDevice);


/**
 * @brief 设置当前ND03的积分时间
 * @param   pNxDevice   模组设备
 * @param   current_exp 当前积分时间/us
 * @return  uint32_t    
 */
int32_t ND03_SetCurrentExp(ND03_Dev_t *pNxDevice, uint16_t current_exp);


/**
 * @brief 获取当前ND03的温度
 *
 * @param   pNxDevice   模组设备
 * @return  int32_t     当前温度/0.1度
 */
int16_t ND03_GetCurrentTherm(ND03_Dev_t *pNxDevice);


/**
 * @brief ND03 Delay 1ms
 * @param   ms 延时时间
 * @return  void  
 */
void ND03_Delay1ms(uint32_t ms);


/**
 * @brief ND03 Delay 10us
 * @param   us 延时时间
 * @return  void  
 */
void ND03_Delay10us(uint32_t us);


/**
 * @brief 设置xshut引脚的电平
 * @param   pNxDevice   模组设备
 * @param   level    xshut引脚电平，0为低电平，1为高电平
 * @return  void  
 */
void ND03_SetXShutPinLevel(ND03_Dev_t *pNxDevice, uint32_t level);


/**
 * @brief 硬件待机api
 * @param   pNxDevice   模组设备
 * @return  void  
 */
void ND03_HardwareSleep(ND03_Dev_t *pNxDevice);


/**
 * @brief 从硬件待机中唤醒api
 * @param   pNxDevice   模组设备
 * @return  void  
 */
void ND03_HardwareWakeup(ND03_Dev_t *pNxDevice);


/*!
 * @brief 软件待机api
 * @param pNxDevice: ND03模组设备信息结构体
 * @return int32_t
*/
int32_t ND03_SoftwareSleep(ND03_Dev_t *pNxDevice);


/*!
 * @brief 从软件待机中唤醒api
 * @param pNxDevice: ND03模组设备信息结构体
 * @return int32_t
*/
int32_t ND03_SoftwareWakeup(ND03_Dev_t *pNxDevice);


/**
 * @brief ND03 InitDevice(初始化设备)
 *
 * @param   pNxDevice   模组设备
 * @return  int32_t
 */
int32_t ND03_InitDevice(ND03_Dev_t *pNxDevice);


/**
 * @brief ND03 Get Device Information (获取当前设备信息)
 *
 * @param   pNxDevice   模组设备
 * @return  void   
 */
int32_t ND03_GetDevInfo(ND03_Dev_t *pNxDevice);


/**
 * @brief ND03 Get Firmware Version (获取ND03模组固件版本号)
 *
 * @param   pNxDevice   模组设备
 * @param   pFirmwareDataBuf  固件版本数据指针
 * @return  int32_t   
 */
int32_t ND03_GetFirmwareVersion(ND03_Dev_t *pNxDevice, uint32_t* pFirmwareDataBuf);


/*!
 * @brief ND03 Get Data from ND03 Flash
 * @param pNxDevice: ND01模组设备信息结构体指针
 * @param flash_addr: flash地址
 * @param flash_rdata: 存放flash数据指针的指针
 * @return int32_t
*/
int32_t ND03_GetFlashData(ND03_Dev_t *pNxDevice, uint32_t flash_addr, uint32_t* flash_rdata);


/**
 * @brief ND03 Wait for Device Boot Up (等待ND03模组启动)
 *
 * @param   pNxDevice   模组设备
 * @return  ND03_ERROR_NONE:成功
 * @return  ND03_ERROR_BOOT:启动失败--请检测模组是否焊接好，还有i2c地址与读写函数是否错误。
 * @return  ND03_ERROR_FW:固件不兼容--请与FAE联系，是否模组的固件与SDK不兼容。
 */
int32_t ND03_WaitDeviceBootUp(ND03_Dev_t *pNxDevice);

/** @} ND03_Dev_Group */

#endif

