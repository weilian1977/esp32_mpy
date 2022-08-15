/**
 * @file nd03_data.c
 * @author tongsheng.tang
 * @brief ND03 get depth data functions
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


int32_t ND03_WaitDepthAndAmpBufReady(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    buf_valid_flag = 0x0;
    uint32_t    i = 2000;


    ND03_Delay1ms(10);
    while(i)
    {
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &buf_valid_flag);
        // NX_PRINTF("%s -> buf_valid_flag2: 0x%08x\r\n", __func__, buf_valid_flag);
        if(0x00 != (buf_valid_flag & ND03_DEPTH_DATA_VAL_MASK))
            break;
        ND03_Delay1ms(2);
        i--;
    }

    if(i == 0)
    {
        return ND03_ERROR_TIME_OUT;
    }

    return ret;
}

int32_t ND03_GetDepthAndAmpBufReady(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    buf_valid_flag = 0x0;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &buf_valid_flag);
    if(ND03_ERROR_NONE != ret)
    {
		NX_PRINTF("ND03_ReadWord failed!!!\r\n");
        return ret;
    }
    // NX_PRINTF("%s -> buf_valid_flag2: 0x%08x\r\n", __func__, buf_valid_flag);
    if(0x00 != (buf_valid_flag & ND03_DEPTH_DATA_VAL_MASK))
        return 1;

    return ret;
}

int32_t ND03_StartMeasurement(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    data_req_flag = 0x0;

    //Set data_req
    ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &data_req_flag);
    data_req_flag = data_req_flag | ND03_DEPTH_DATA_REQ_MASK;
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, data_req_flag); //发送触发信号

    return ret;
}


int32_t ND03_StopMeasurement(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    data_req_flag = 0x0;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &data_req_flag);
    data_req_flag = data_req_flag & (~ND03_DEPTH_DATA_REQ_MASK);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, data_req_flag);

    return ret;
}


int32_t ND03_ReadDepthAndAmpData(ND03_Dev_t *pNxDevice, ND03_RangingData_t *pData)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_AMP_DEPTH, (uint32_t*)&pData->depth);

    return 0;
}


int32_t ND03_ClearDataValidFlag(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    data_valid_flag;
    
    ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &data_valid_flag);
    data_valid_flag = data_valid_flag & (~ND03_DEPTH_DATA_VAL_MASK);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, data_valid_flag);

    return ret;
}


int32_t ND03_GetRangingData(ND03_Dev_t *pNxDevice, ND03_RangingData_t *pData)
{
    int32_t     ret = ND03_ERROR_NONE;

    /* 等待测量完成 */
    ret |= ND03_WaitDepthAndAmpBufReady(pNxDevice);
    /* 读取测量数据 */
    ret |= ND03_ReadDepthAndAmpData(pNxDevice, pData);
    /* 清除数据有效标志位 */
    ret |= ND03_ClearDataValidFlag(pNxDevice);

    return ret;
}

