/**
 * @file nd03_comm.c
 * @author tongsheng.tang
 * @brief ND03 communication functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */

#include "nd03_comm.h"
#include "nd03_dev.h"
#include "nd03_data.h"
#include "nd03_calib.h"


/** iic单字节写函数指针 */
int32_t(*I2C_WriteNBytesLib)(uint8_t, uint16_t, uint8_t *, uint8_t);
/** iic单字节读函数指针 */
int32_t(*I2C_ReadNBytesLib)(uint8_t, uint16_t,uint8_t *, uint8_t);


void ND03_RegisteredIICApi(
        int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t),
        int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t)
    )
{
    I2C_ReadNBytesLib = I2C_ReadNBytesFunc;
    I2C_WriteNBytesLib = I2C_WriteNBytesFunc;
}


int32_t ND03_WriteByte(ND03_Dev_t *pNxDevice, uint16_t addr, uint8_t wdata)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret = I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, &wdata, 1);

	return ret;
}


int32_t ND03_WriteWord(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t wdata)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint8_t     data[4];

    data[0] = (wdata >> 0) & 0xff;
    data[1] = (wdata >> 8) & 0xff;
    data[2] = (wdata >>16) & 0xff;
    data[3] = (wdata >>24) & 0xff;
    ret = I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, data, 4);

	return ret;
}


int32_t ND03_ReadWord(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint8_t     data[4];

    ret = I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, addr, data, 4);
    *rdata = ((uint32_t)data[3]<<24) | ((uint32_t)data[2]<<16) | ((uint32_t)data[1]<<8) | (uint32_t)data[0];

	return ret;
}


int32_t ND03_WriteNWords(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *wdata, uint32_t len)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret |= I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, (uint8_t*)wdata, len*4);

	return ret;
}


int32_t ND03_ReadNWords(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata, uint32_t len)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret |= I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, addr, (uint8_t*)rdata, len*4);

	return ret;
}

