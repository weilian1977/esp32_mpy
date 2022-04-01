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

/*! 
 @mainpage  ND03_SDK
 <table>
 <tr><th>Project  <td>ND03_SDK 
 <tr><th>Author   <td>tongsheng.tang
 <tr><th>Date     <td>2021-11
 </table>
 @section   项目详细描述
 此项目基于ND03模组进行开发的全功能SDK，<br/>

 
 @section	包含功能
 -#	模组的各种配置功能
 -#	模组两种标定功能
 -# 模组的测距功能

 @section   SDK版本 
 <table>
 <tr><th>Date              <th>S_Version      <th>Author              <th>Description  </tr>
 <tr><td>2021/11    	   <td>V1.0.0         <td>tongsheng.tang      <td>创建初始版本 </tr>
 <tr><td>2022/01    	   <td>V1.0.3         <td>yan.huang           <td>修改注释     </tr>
 <tr><td>2022/01    	   <td>V1.0.4         <td>yan.huang           <td>
 -# 新增注释
 -# 修改README文档 
 -# 修改判断设备进入工作状态标志
 </tr>     
 </table>
*/


#include "nd03_comm.h"
#include "nd03_dev.h"
#include "nd03_data.h"
#include "nd03_calib.h"
#include "nd03_def.h"


/** SDK主版本 */
static uint8_t sdk_version_major = 1;
/** SDK次版本 */
static uint8_t sdk_version_minor = 0;
/** SDK小版本 */
static uint8_t sdk_version_patch = 4;

//Function ptr
/** 延时1ms函数指针 */
void(*Delay1msLib)(uint32_t );
/** 延时10us函数指针 */
void(*Delay10usLib)(uint32_t );


uint32_t ND03_GetSdkVersion(void)
{
    return (uint32_t)sdk_version_major * 10000 + (uint32_t)sdk_version_minor * 100 + (uint32_t)sdk_version_patch;
}


void ND03_RegisteredPlatformApi(ND03_Func_Ptr_t dev_op)
{
    ND03_RegisteredIICApi(dev_op.I2C_WriteNBytesFunc, dev_op.I2C_ReadNBytesFunc);
    Delay1msLib = dev_op.Delay1msFunc;
    Delay10usLib = dev_op.Delay10usFunc;
}


int32_t ND03_SetRangeMode(ND03_Dev_t *pNxDevice, uint32_t mode)
{
    int32_t     ret = ND03_ERROR_NONE;

    // //Set mode reg
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_RANGE_MODE, mode);

    if(ret == ND03_ERROR_NONE)
    {
        pNxDevice->config.range_mode = mode;

        if((mode & ND03_DEVICEMODE_MASK) == ND03_DEVICEMODE_CONTINUOUS_RANGING)
            ND03_StartMeasurement(pNxDevice);
    }

    return ret;
}


uint32_t ND03_GetRangeMode(ND03_Dev_t *pNxDevice)
{
    return pNxDevice->config.range_mode;
}


int32_t ND03_SetSlaveAddr(ND03_Dev_t *pNxDevice, uint8_t addr)
{
	int32_t ret = ND03_ERROR_NONE;

	ret = ND03_WriteWord(pNxDevice, ND03_REG_SLAVE_ADDR, (uint32_t)addr);

    if(ret == ND03_ERROR_NONE)
	    pNxDevice->i2c_dev_addr = addr;

    return ret;
}


int32_t ND03_GetSlaveAddr(ND03_Dev_t *pNxDevice, uint8_t *addr)
{
    int32_t ret = ND03_ERROR_NONE;

    *addr = pNxDevice->i2c_dev_addr;

    return ret;
}


int32_t ND03_SetLutState(ND03_Dev_t *pNxDevice, ND03_Status_e flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(flag != ND03_DISABLE)
        rbuf = rbuf | ND03_LUT_TOF_FLAG;
    else
        rbuf = rbuf & (~ND03_LUT_TOF_FLAG);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_TOF_FLAG, rbuf); 

    return ret;
}


int32_t ND03_GetLutState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        *flag = ((rbuf & ND03_LUT_TOF_FLAG) == ND03_LUT_TOF_FLAG) ? ND03_ENABLE : ND03_DISABLE;
    }
    
    return ret;
}


int32_t ND03_SetAutoExpState(ND03_Dev_t *pNxDevice, ND03_Status_e flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t     rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(flag != ND03_DISABLE)
        rbuf = rbuf | ND03_AE_TOF_FLAG;
    else
        rbuf = rbuf & (~ND03_AE_TOF_FLAG);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_TOF_FLAG, rbuf); 

    return ret;
}


int32_t ND03_GetAutoExpState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t     rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        *flag = ((rbuf & ND03_AE_TOF_FLAG) == ND03_AE_TOF_FLAG) ? ND03_ENABLE : ND03_DISABLE;
    }

    return ret;
}


int32_t ND03_SetXTalkState(ND03_Dev_t *pNxDevice, ND03_Status_e flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(flag != ND03_DISABLE)
        rbuf = rbuf | ND03_XT_TOF_FLAG;
    else
        rbuf = rbuf & (~ND03_XT_TOF_FLAG);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_TOF_FLAG, rbuf); 

    return ret;
}


int32_t ND03_GetXTalkState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        *flag = ((rbuf & ND03_XT_TOF_FLAG) == ND03_XT_TOF_FLAG) ? ND03_ENABLE : ND03_DISABLE;
    }
    
    return ret;
}


int32_t ND03_SetThermCompensationState(ND03_Dev_t *pNxDevice, ND03_Status_e flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t     rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(flag != ND03_DISABLE)
        rbuf = rbuf | ND03_THERM_TOF_FLAG;
    else
        rbuf = rbuf & (~ND03_THERM_TOF_FLAG);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_TOF_FLAG, rbuf); 

    return ret;
}


int32_t ND03_GetThermCompensationState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        *flag = ((rbuf & ND03_THERM_TOF_FLAG) == ND03_THERM_TOF_FLAG) ? ND03_ENABLE : ND03_DISABLE;
    }
    
    return ret;
}


int32_t ND03_SetGpioConfig(ND03_Dev_t *pNxDevice, ND03_GpioFunc_t functionality, ND03_GpioPolar_t polarity)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_GPIO_SETTING, &rbuf);
    //Setting functionality
    rbuf = rbuf & (~GPIO_FUNCTIONALITY);
    rbuf = rbuf | (functionality & GPIO_FUNCTIONALITY);
    //Setting polarity
    if(polarity == ND03_GPIO_POLARITY_HIGH)
        rbuf = rbuf | GPIO_POLARITY_FLAG;
    else
        rbuf = rbuf & (~GPIO_POLARITY_FLAG);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_GPIO_SETTING, rbuf);

    return ret;
}


int32_t ND03_GetGpioConfig(ND03_Dev_t *pNxDevice, ND03_GpioFunc_t *functionality, ND03_GpioPolar_t *polarity)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_GPIO_SETTING, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        // Polarity
        *polarity = ((rbuf & GPIO_POLARITY_FLAG) == GPIO_POLARITY_FLAG) ? ND03_GPIO_POLARITY_HIGH : ND03_GPIO_POLARITY_LOW;
        // Functionality
        *functionality = rbuf & GPIO_FUNCTIONALITY;
    }
    
    return ret;
}


int32_t ND03_SetDepthThreshold(ND03_Dev_t *pNxDevice, uint16_t depth_low, uint16_t depth_high)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    rbuf = ((uint32_t)depth_high<<16) | (uint32_t)depth_low;
    ret = ND03_WriteWord(pNxDevice, ND03_REG_GPIO_DEPTH_TH, rbuf);

    return ret;
}


int32_t ND03_GetDepthThreshold(ND03_Dev_t *pNxDevice, uint16_t *depth_low, uint16_t *depth_high)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;


    ret = ND03_ReadWord(pNxDevice, ND03_REG_GPIO_DEPTH_TH, &rbuf);
    if(ND03_ERROR_NONE == ret)
    {
        *depth_low = rbuf & 0xFFFF;
        *depth_high = (rbuf>>16) & 0xFFFF;
    }

    return ret;
}


int32_t ND03_SetMaxExposure(ND03_Dev_t *pNxDevice, uint16_t max_exposure_us)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret = ND03_WriteWord(pNxDevice, ND03_REG_MAX_EXPOSURE, (uint32_t)max_exposure_us);

    return ret;
}


int32_t ND03_GetMaxExposure(ND03_Dev_t *pNxDevice, uint16_t *max_exposure_us)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_MAX_EXPOSURE, &rbuf);
    if(ND03_ERROR_NONE == ret)
    {
        *max_exposure_us = (uint16_t)rbuf;
    }

    return ret;
}


uint16_t ND03_GetCurrentExp(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;
    uint16_t    current_exp = 0;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_EXP_THERM, &rbuf);
    if(ND03_ERROR_NONE == ret){
        current_exp = (rbuf >> 16) & 0xFFFF;
    }

	return current_exp;
}


int32_t ND03_SetCurrentExp(ND03_Dev_t *pNxDevice, uint16_t current_exp)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_EXP_THERM, &rbuf);

    if(ND03_ERROR_NONE == ret){
        rbuf &= 0x0000FFFF;
        rbuf |= ((uint32_t)current_exp << 16);
        ret = ND03_WriteWord(pNxDevice, ND03_REG_EXP_THERM, rbuf);
    }

	return current_exp;
}


int16_t ND03_GetCurrentTherm(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;
    int16_t     current_therm = 0;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_EXP_THERM, &rbuf);
    if(ND03_ERROR_NONE == ret){
        current_therm = rbuf & 0xFFFF;
    }

	return current_therm;
}


void ND03_Delay1ms(uint32_t ms)
{
    Delay1msLib(ms);
} 

void ND03_Delay10us(uint32_t us)
{
    Delay10usLib(us);
} 


void ND03_SetXShutPinLevel(ND03_Dev_t *pNxDevice, uint32_t level)
{
    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        pNxDevice->SetXShutPinLevelFunc(level);
} 


void ND03_HardwareSleep(ND03_Dev_t *pNxDevice)
{
    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        pNxDevice->SetXShutPinLevelFunc(0);
} 


void ND03_HardwareWakeup(ND03_Dev_t *pNxDevice)
{
    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        pNxDevice->SetXShutPinLevelFunc(1);
} 


int32_t ND03_SoftwareSleep(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_SETTING, ND03_SETTING_SW_SLEEP_MASK);

    return ret;
}


int32_t ND03_SoftwareWakeup(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_SETTING, &rbuf);

    return ret;
}


int32_t ND03_InitDevice(ND03_Dev_t *pNxDevice)
{
	int32_t     ret = ND03_ERROR_NONE;

    NX_PRINTF("ND03_InitDevice Start!\r\n");

    ret |= ND03_StopMeasurement(pNxDevice);

	ret |= ND03_GetDevInfo(pNxDevice);

    NX_PRINTF("ND03_InitDevice End!\r\n");

	return ret;
}


int32_t ND03_GetFirmwareVersion(ND03_Dev_t *pNxDevice, uint32_t* pFirmwareDataBuf)
{
	int32_t     ret = ND03_ERROR_NONE;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_FW_VERSION, pFirmwareDataBuf);

	return ret;
}


int32_t ND03_GetFlashData(ND03_Dev_t *pNxDevice, uint32_t flash_addr, uint32_t* flash_rdata)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;
    uint16_t    retry_cnt = 500;

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_FLASH_ADDR, flash_addr);
    ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
    rbuf = rbuf | ND03_FLASH_DATA_REQ_MASK;
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, rbuf);
    ND03_Delay1ms(2);
    do{
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
		ND03_Delay1ms(2);
	}while(((rbuf & ND03_FLASH_DATA_VAL_MASK) == 0) && (-- retry_cnt));

    if(retry_cnt == 0)
    {
        return ND03_ERROR_TIME_OUT;
    }

    rbuf = rbuf & (~ND03_FLASH_DATA_VAL_MASK);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, rbuf);

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_FLASH_DATA, flash_rdata);

    return ret;
}


int32_t ND03_GetDevInfo(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    // //read nd03 version
	ret |= ND03_GetFirmwareVersion(pNxDevice, &pNxDevice->chip_info.nd03_fw_version);
	NX_PRINTF("FwVersion: %d\r\n", pNxDevice->chip_info.nd03_fw_version);

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_RANGE_MODE, &pNxDevice->config.range_mode);

    // // //read nd03 product date
    // ret |= ND03_GetFlashData(pNxDevice, 4, &pNxDevice->chip_info.nd03_product_date);
    // NX_PRINTF("Product Date: %d(0x%x)\r\n", pNxDevice->chip_info.nd03_product_date, pNxDevice->chip_info.nd03_product_date);

    // // //read nd03 product time
    // ret |= ND03_GetFlashData(pNxDevice, 8, &pNxDevice->chip_info.nd03_product_time);
    // NX_PRINTF("Product Time: %d(0x%x)\r\n", pNxDevice->chip_info.nd03_product_time, pNxDevice->chip_info.nd03_product_time);

    // // //read nd03 model revision
    // ret |= ND03_GetFlashData(pNxDevice, 8, &pNxDevice->chip_info.nd03_model_revision);
    // NX_PRINTF("Product Time: %d(0x%x)\r\n", pNxDevice->chip_info.nd03_model_revision, pNxDevice->chip_info.nd03_model_revision);

    // // //read nd03 chip id
    // NX_PRINTF("Product Time: ");
    // for(i = 0; i < sizeof(pNxDevice->chip_info.nd03_chip_id)/sizeof(uint32_t); i++){

    //     ret |= ND03_GetFlashData(pNxDevice, i, &rbuf);
    //     *((uint32_t*)&pNxDevice->chip_info.nd03_chip_id[i*4]) = rbuf;
    //     NX_PRINTF("%c%c%c%c", (rbuf>>24)&0xFF, (rbuf>>16)&0xFF, (rbuf>>8)&0xFF, (rbuf>>0)&0xFF);
    // }
    
    return ret;
}


int32_t ND03_WaitDeviceBootUp(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    int32_t     try_times = 2000;
    uint32_t    rbuf;

    NX_PRINTF("%s Start!\r\n", __func__);

    /* 重启 */
    ND03_HardwareSleep(pNxDevice);
	ND03_Delay10us(2);
    ND03_HardwareWakeup(pNxDevice);
	ND03_Delay10us(50);

    /* 判断是否完成启动项配置 */
    ND03_ReadWord(pNxDevice, ND03_REG_DEV_ID, &rbuf);
    NX_PRINTF("ND03_REG_DEVICE_ID: 0x%08x\r\n", rbuf);

    /* 进行启动项配置 */
    if(rbuf != ND03_REG_DEV_ID_VALUE)
    {
        ret |= ND03_WriteByte(pNxDevice, ND03_REG_BOOT1, ND03_REG_BOOT1_VALUE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_BOOT2, ND03_REG_BOOT2_VALUE);
        ND03_Delay10us(30);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_BOOT3, ND03_REG_BOOT3_VALUE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_BOOT4, ND03_REG_BOOT4_VALUE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_STATE, ND03_STATE_IDLE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_BOOT5, ND03_REG_BOOT5_VALUE);
    }

    /* 等待进入工作状态 */
    do{
        ND03_Delay10us(50);
		ret |= ND03_ReadWord(pNxDevice, ND03_REG_STATE, &pNxDevice->dev_pwr_state);
	}while((pNxDevice->dev_pwr_state != ND03_STATE_SOFTWARE_READY) && (pNxDevice->dev_pwr_state != ND03_STATE_GOT_DEPTH)   \
            && --try_times);

	if((ND03_ERROR_NONE != ret) || (0 == try_times))
    {
        NX_PRINTF("ND03 boot error\r\n");

        return ND03_ERROR_BOOT;
    }
    
    NX_PRINTF("%s End!\r\n", __func__);

    return ND03_ERROR_NONE;
}
