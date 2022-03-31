/*
    This file list all the functions for user to implement.
*/

#ifndef __ND03_PLATFORM__H__
#define __ND03_PLATFORM__H__
#include <stdint.h>
#include "nd03_def.h"

/**
 * @brief I2C读一个字节
 * 
 * @param   i2c_addr   从机设备地址
 * @param   i2c_read_addr  读寄存器地址
 * @param   i2c_read_data  读数据
 * @return  int32_t   
 * @retval  0:成功, 其他:失败
 */
int32_t i2c_read_nbytes(uint8_t i2c_addr, uint16_t i2c_read_addr, uint8_t *i2c_read_data, uint8_t len);

/**
 * @brief I2C写一个字节
 * 
 * @param   i2c_addr   从机设备地址
 * @param   i2c_write_addr  写寄存器地址
 * @param   i2c_write_data  写数据
 * @return  int32_t   
 * @retval  0:成功, 其他:失败
 */
int32_t i2c_write_nbytes(uint8_t i2c_addr, uint16_t i2c_write_addr, uint8_t *i2c_write_data, uint8_t len);

/**
 * @brief 延时count毫秒
 *
 * @param   count  输入需要延时时长  
 * @return  void   
 */
void delay_1ms(uint32_t count);

/**
 * @brief 延时10*count微秒
 *
 * @param   count  输入需要延时时长
 * @return  void   
 */
void delay_10us(uint32_t count);


/**
 * @brief 设置nd03 xshut引脚电平
 *
 * @param   level  引脚电平，0为低电平，1为高电平
 * @return  void   
 */
void set_xshut_pin_level(uint32_t level);

#endif

