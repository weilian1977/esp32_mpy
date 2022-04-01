/**
 * @file nd03_calib.h
 * @author tongsheng.tang
 * @brief ND03 Calibration functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */

#ifndef __ND03_CALIB_H__
#define __ND03_CALIB_H__

#include "nd03_def.h"

/** @defgroup ND03_Calibration_Group ND03 Calibration Funtions
 *  @brief ND03 Calibration Funtions
 *  @{
 */


/*!
 * @brief ND03 Read Error Status 读取错误寄存器状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t: 如果通信正常，则返回错误寄存器状态值
*/
int32_t ND03_ReadErrorStatus(ND03_Dev_t *pNxDevice);


/*!
 * @brief ND03 Calibration Request 发送标定请求
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t
*/
int32_t ND03_CalibrationRequest(ND03_Dev_t *pNxDevice);


/*!
 * @brief ND03 XTalkCalibration Request 发送串扰标定请求
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t
*/
int32_t ND03_XTalkCalibrationRequest(ND03_Dev_t *pNxDevice);


/*!
 * @brief ND03 Wait for Calibration 等待标定的完成
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t
*/
int32_t ND03_WaitforCalibration(ND03_Dev_t *pNxDevice);


/*!
 * @brief ND03 Wait for XTalk Calibration 等待串扰标定的完成
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t
*/
int32_t ND03_WaitforXTalkCalibration(ND03_Dev_t *pNxDevice);


/**
 * @brief ToF非线性标定
 * 
 * @param   pNxDevice   设备模组
 * @return  int32_t  
 * @retval  0:  成功
 * @retval  17: 数据非线性错误
 * @retval  18: 数据抖动错误
 */
int32_t ND03_Calibration(ND03_Dev_t *pNxDevice);


/**
 * @brief 串扰标定功能
 * 
 * @note  矫正玻璃盖板串扰
 * 
 * @param   pNxDevice   模组设备       
 * @return  int32_t   
 * @retval  0:  成功
 * @retval  !0: 串扰标定失败
 */
int32_t ND03_XTalkCalibration(ND03_Dev_t *pNxDevice);


/**
 * @brief 设置标定盒子长度
 * @param   pNxDevice 模组设备
 * @param   dist_mm   非线性标定长度 / mm
 * @return  int32_t   
 */
int32_t ND03_SetNonLinearCalibDistMM(ND03_Dev_t *pNxDevice, uint16_t dist_mm);


/**
 * @brief 设置二次标定的距离
 * 
 * @note  建议取值范围在60~80cm，推荐使用80cm
 * 
 * @param   pNxDevice   模组设备
 * @param   dist_mm   二次标定距离长度 / mm
 * @return  int32_t
 */
int32_t ND03_SetXTalkCalibDistMM(ND03_Dev_t *pNxDevice, uint16_t dist_mm);


/** @} ND03_Calibration_Group */

#endif

