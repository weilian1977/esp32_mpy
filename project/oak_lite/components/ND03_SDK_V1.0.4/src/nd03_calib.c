/**
 * @file nd03_calib.c
 * @author tongsheng.tang
 * @brief ND03 Calibration functions
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


int32_t ND03_ReadErrorStatus(ND03_Dev_t *pNxDevice){
    int32_t     ret = ND03_ERROR_NONE;
    int32_t     error;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_ERROR_FLAG, (uint32_t*)&error);
    if(ND03_ERROR_NONE == ret){
        ret = error;
        NX_PRINTF("Error: %d\r\n", error);
    }

    return ret;
}


int32_t ND03_CalibrationRequest(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
    rbuf = rbuf | ND03_CALIBRATION_REQ_MASK;
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, rbuf);

    return ret;
}


int32_t ND03_XTalkCalibrationRequest(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
    rbuf = rbuf | ND03_XTALK_CALIB_REQ_MASK;
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, rbuf);

    return ret;
}


int32_t ND03_WaitforCalibration(ND03_Dev_t *pNxDevice)
{
	int32_t     ret = ND03_ERROR_NONE;
	uint32_t    rbuf = 0;
    uint16_t    retry_cnt = 500;
	
    do{
		ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
		ND03_Delay1ms(500);
	}while(((rbuf & ND03_CALIBRATION_VAL_MASK) == 0) && (-- retry_cnt));

    if(retry_cnt == 0)
    {
        NX_PRINTF("Calibration Error\r\n");
        return ND03_ERROR_TIME_OUT;
    }

    rbuf = rbuf & (~ND03_CALIBRATION_VAL_MASK);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, rbuf);

    return ret;
}


int32_t ND03_WaitforXTalkCalibration(ND03_Dev_t *pNxDevice)
{
	int32_t     ret = ND03_ERROR_NONE;
	uint32_t    rbuf = 0;
    uint16_t    retry_cnt = 100;

	
    do{
		ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
		ND03_Delay1ms(500);
	}while(((rbuf & ND03_XTALK_CALIB_VAL_MASK) == 0) && (-- retry_cnt));

    if(retry_cnt == 0)
    {
        return ND03_ERROR_TIME_OUT;
    }

    rbuf = rbuf & (~ND03_XTALK_CALIB_VAL_MASK);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, rbuf);

    return ret;
}


int32_t ND03_Calibration(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    NX_PRINTF("\r\n%s Start!\r\n", __func__);

    // ret |= ND03_SetNonLinearCalibDistMM(pNxDevice, 150);    // default calibration depth
    ret |= ND03_CalibrationRequest(pNxDevice);
    ND03_Delay1ms(3000);
    ret |= ND03_WaitforCalibration(pNxDevice);
    ret |= ND03_ReadErrorStatus(pNxDevice);

    NX_PRINTF("%s End!\r\n", __func__);

    return ret;
}


int32_t ND03_XTalkCalibration(ND03_Dev_t *pNxDevice)
{
	int32_t     ret = ND03_ERROR_NONE;
	
    NX_PRINTF("\r\n%s Start!\r\n", __func__);

    // ret |= ND03_SetXTalkCalibDistMM(pNxDevice, 800);      // default xtalk calibration depth
    ret |= ND03_XTalkCalibrationRequest(pNxDevice);
    ND03_Delay1ms(3000);
    ret |= ND03_WaitforXTalkCalibration(pNxDevice);
    ret |= ND03_ReadErrorStatus(pNxDevice);

    NX_PRINTF("%s End!\r\n", __func__);
	
	return ret;
}


int32_t ND03_SetNonLinearCalibDistMM(ND03_Dev_t *pNxDevice, uint16_t dist_mm)
{
	int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;


    ret |= ND03_ReadWord(pNxDevice, ND03_REG_CALIB_OFFSET, &rbuf);
    rbuf = (rbuf & 0xFFFF0000) + ((uint32_t)dist_mm & 0x0000FFFF);
	ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_OFFSET, rbuf);

	return ret;
}


int32_t ND03_SetXTalkCalibDistMM(ND03_Dev_t *pNxDevice, uint16_t dist_mm)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_CALIB_OFFSET, &rbuf);
    rbuf = (rbuf & 0x0000FFFF) + (((uint32_t)dist_mm<<16) & 0xFFFF0000);
	ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_OFFSET, rbuf);

	return ret;
}

