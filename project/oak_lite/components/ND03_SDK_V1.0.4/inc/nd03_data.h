/**
 * @file nd03_data.h
 * @author tongsheng.tang
 * @brief ND03 get depth data functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND03_DATA_H__
#define __ND03_DATA_H__

#include "nd03_def.h"
 
/** @defgroup ND03_Data_Group ND03 Data Funtions
 *  @brief ND03 Data Funtions
 *  @{
 */

/**
 * @brief ND03 Wait Depth And Amp Buf Ready (等待ND03准备好数据)
 *
 * @param   pNxDevice 模组设备
 * @return  int32_t  
 * @retval  函数执行结果
 * - 0：正常返回
 * - ::ND03_ERROR_TIME_OUT:获取数据超时
 */
int32_t ND03_WaitDepthAndAmpBufReady(ND03_Dev_t *pNxDevice);

/**
 * @brief ND03 Get Depth And Amp Buf Ready (ND03检测获取数据)
 *
 * @param   pNxDevice 模组设备
 * @return  int32_t  
 * @retval  函数执行结果
 * - 0：正常返回
 * - ::ND03_ERROR_I2C:IIC通讯错误/数据异常
 */
int32_t ND03_GetDepthAndAmpBufReady(ND03_Dev_t *pNxDevice);

/**
 * @brief ND03 Start Measurement (发送开始测量信号)
 *
 * @param   pNxDevice 模组设备
 * @return  int32_t   
 */
int32_t ND03_StartMeasurement(ND03_Dev_t *pNxDevice);


/**
 * @brief ND03 Stop Measurement (发送结束测量信号)
 *
 * @param   pNxDevice 模组设备
 * @return  int32_t   
 */
int32_t ND03_StopMeasurement(ND03_Dev_t *pNxDevice);


/**
 * @brief ND03 Clear Data Valid Flag (清除ND03测量数据的有效位，取完一次数据做的操作，通知ND03做下一次测量)
 *
 * @param   pNxDevice 模组设备
 * @return  int32_t   
 */
int32_t ND03_ClearDataValidFlag(ND03_Dev_t *pNxDevice);


/**
 * @brief ND03 Get Ranging Data (从ND03中获取一次深度数据，需要与ND03_StartMeasurement函数搭配)
 *
 * @param   pNxDevice 模组设备
 * @param   pData  获取到的深度和幅度数据
 * @return  int32_t   
 */
int32_t ND03_GetRangingData(ND03_Dev_t *pNxDevice, ND03_RangingData_t *pData);


/**
 * @brief ND03 Read Depth And Amp Data (读取ND03寄存器获取深度幅度数据)
 *
 * @param   pNxDevice 模组设备
 * @param   pData  获取到的深度和幅度数据
 * @return  int32_t   
 */
int32_t ND03_ReadDepthAndAmpData(ND03_Dev_t *pNxDevice, ND03_RangingData_t *pData);

/** @} ND03_Data_Group */

#endif

