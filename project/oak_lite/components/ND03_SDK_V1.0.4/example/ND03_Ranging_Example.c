/**
 * @file nd03_dev.c
 * @author tongsheng.tang
 * @brief ND03 device setting functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */

#include <stdio.h>
#include "nd03_platform.h"
#include "nd03_comm.h"
#include "nd03_dev.h"
#include "nd03_data.h"
#include "nd03_calib.h"
#include "nd03_def.h"

/** 模组设备 */
ND03_Dev_t g_nd03_device = {.i2c_dev_addr = ND03_DEFAULT_SLAVE_ADDR, .SetXShutPinLevelFunc = set_xshut_pin_level};

/**
 * @brief 单次测量接口
 * 
 * @param pNxDevice 模组设备
 */
void SG_RangingTest(ND03_Dev_t *pNxDevice)
{
    ND03_RangingData_t data;
    /* 请求一次测量 */
    ND03_StartMeasurement(pNxDevice);
    /* 获取测量数据 */
    ND03_GetRangingData(pNxDevice, &data);
    NX_PRINTF("depth:%d mm, amp:%d, exp:%d\r\n", data.depth, data.amp, ND03_GetCurrentExp(pNxDevice));
}

/**
 * @brief ND03 single ranging example main function
 * 
 * @return int32_t 
 */
int32_t main(void)
{
	/* 函数指针结构体 */
    ND03_Func_Ptr_t dev_op = {NULL, NULL, NULL, NULL}; 

	/* 循环测距次数为100次 */
    int32_t cnt = 100;
    
	/* 初始化函数指针结构体 */
	dev_op.Delay10usFunc = delay_10us;
	dev_op.Delay1msFunc = delay_1ms;
	dev_op.I2C_ReadNBytesFunc = i2c_read_nbytes;
	dev_op.I2C_WriteNBytesFunc = i2c_write_nbytes;

	/* 将host端功能函数注册到SDK中 */
    ND03_RegisteredPlatformApi(dev_op);

	/* 初始化设备变量的IIC地址 */
    g_nd03_device.i2c_dev_addr = ND03_DEFAULT_SLAVE_ADDR;

    /* 循环等待设备启动, 若模组或者IIC读写函数有问题则会报错 */
	if(ND03_ERROR_NONE != ND03_WaitDeviceBootUp(&g_nd03_device))
	{
		NX_PRINTF("ND03_WaitDeviceBootUp error\r\n");
		return -1;
	}

	/* 初始化模组设备 */
	if(ND03_ERROR_NONE != ND03_InitDevice(&g_nd03_device))
	{
		NX_PRINTF("ND03_InitDevice error!!\r\n");
		return -1;
	}

	/* 循环测量 */
	while(cnt --) {
        SG_RangingTest(&g_nd03_device);
    }

	return 0;
}

