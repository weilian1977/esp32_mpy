/**
 * @file nd03_comm.h
 * @author tongsheng.tang
 * @brief ND03 communication functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND03_COMM_H__
#define __ND03_COMM_H__

#include "nd03_def.h"

/** @defgroup ND03_Communication_Group ND03 Communication Funtions
 *  @brief ND03 Communication Funtions
 *  @{
 */

/**
 * @brief 注册IIC API
 *
 * @param   i2c_write_byte_func iic 写一个byte函数
 * @param   i2c_read_byte_func  iic 读一个byte函数
 * @return  void
 */
void ND03_RegisteredIICApi(
        int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t len),
        int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t len)
    );


/*!
 * @brief ND03 Write 1 Byte to ND03
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param wdata: 寄存器的值
 * @return int32_t
*/
int32_t ND03_WriteByte(ND03_Dev_t *pNxDevice, uint16_t addr, uint8_t wdata);


/*!
 * @brief ND03 Write 4 Bytes to ND03
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param wdata: 寄存器的值
 * @return int32_t
*/
int32_t ND03_WriteWord(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t wdata);


/*!
 * @brief ND03 Read 4 Bytes from ND03
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param rdata: 存放寄存器值的指针
 * @return int32_t
*/
int32_t ND03_ReadWord(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata);


/*!
 * @brief ND03 Write n Words to ND03
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param wdata: 存放寄存器值的指针
 * @param len: 写数据的长度
 * @return int32_t
*/
int32_t ND03_WriteNWords(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *wdata, uint32_t len);


/*!
 * @brief ND03 Read n Words from ND03
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param rdata: 存放寄存器值的指针
 * @param len: 读数据的长度
 * @return int32_t
*/
int32_t ND03_ReadNWords(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata, uint32_t len);


/*!
 * @brief 检测host大小端与sdk的大小端配置是否一致
 * @return int32_t 
 * @retval 0：成功 其他：失败
*/
int32_t ND03_CheckHostEndian(void);


/** @} ND03_Communication_Group */

#endif

