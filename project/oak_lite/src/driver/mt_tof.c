#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "esp_log.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_err.h"

#include "mt_module_config.h"
#include "mt_tof.h"

#include "esp_log.h"

#include "drv_i2c.h"

#include "nd03_comm.h"
#include "nd03_dev.h"
#include "nd03_data.h"
#include "nd03_calib.h"
#include "nd03_def.h"

#if MODULE_TOF_ENABLE
/************************************************************/
#define TAG                           ("mt_tof")

#define TOF_ND03_SLAVE_ADDR_L			ND03_DEFAULT_SLAVE_ADDR
#define TOF_ND03_SLAVE_ADDR_H			ND03_DEFAULT_SLAVE_ADDR
#define I2C_MASTER_NUM I2C_NUM_1

float distance_tof;

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

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS  
 ******************************************************************************/


/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
/**
 * @brief I2C读一个字节
 * 
 * @param   i2c_addr   从机设备地址
 * @param   i2c_read_addr  读寄存器地址
 * @param   i2c_read_data  读数据
 * @return  int32_t   
 * @retval  0:成功, 其他:失败
 */
static int32_t i2c_read_nbytes(uint8_t i2c_addr, uint16_t i2c_read_addr, uint8_t *i2c_read_data, uint8_t len);

/**
 * @brief I2C写一个字节
 * 
 * @param   i2c_addr   从机设备地址
 * @param   i2c_write_addr  写寄存器地址
 * @param   i2c_write_data  写数据
 * @return  int32_t   
 * @retval  0:成功, 其他:失败
 */
static int32_t i2c_write_nbytes(uint8_t i2c_addr, uint16_t i2c_write_addr, uint8_t *i2c_write_data, uint8_t len);

/**
 * @brief 延时count毫秒
 *
 * @param   count  输入需要延时时长  
 * @return  void   
 */
static void delay_1ms(uint32_t count);

/**
 * @brief 延时10*count微秒
 *
 * @param   count  输入需要延时时长
 * @return  void   
 */
static void delay_10us(uint32_t count);


/**
 * @brief 设置nd03 xshut引脚电平
 *
 * @param   level  引脚电平，0为低电平，1为高电平
 * @return  void   
 */
static void set_xshut_pin_level(uint32_t level);

/** 模组设备 */
ND03_Dev_t g_nd03_device = {.i2c_dev_addr = ND03_DEFAULT_SLAVE_ADDR, .SetXShutPinLevelFunc = set_xshut_pin_level};

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
mt_err_t mt_tof_init_t(void)
{

  /* 函数指针结构体 */
  ND03_Func_Ptr_t dev_op = {NULL, NULL, NULL, NULL};

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
		//NX_PRINTF("ND03_WaitDeviceBootUp error!\r\n");
		return -1;
	}

	/* 初始化模组设备 */
	if(ND03_ERROR_NONE != ND03_InitDevice(&g_nd03_device))
	{
		//NX_PRINTF("ND03_InitDevice error!!\r\n");
		return -1;
	}
  return MT_OK;
}

mt_err_t mt_tof_update_t(void)
{
  ND03_RangingData_t data;
  /* 请求一次测量 */
  ND03_StartMeasurement(&g_nd03_device);
  /* 获取测量数据 */
  ND03_GetRangingData(&g_nd03_device, &data);
  //NX_PRINTF("depth:%d mm, amp:%d, exp:%d\r\n", data.depth, data.amp, ND03_GetCurrentExp(&g_nd03_device));
  distance_tof = data.depth;
  return MT_OK;
}

mt_err_t mt_tof_get_distance_t(float *distance)
{
  (*distance) = distance_tof;
  return MT_OK;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
static int32_t i2c_read_nbytes(uint8_t i2c_addr, uint16_t i2c_read_addr, uint8_t *i2c_read_data, uint8_t len)
{
    //For user implement
    i2c_master_read_mem16(I2C_MASTER_NUM, ND03_DEFAULT_SLAVE_ADDR, i2c_read_addr, i2c_read_data, len);
    return 0;
}

static int32_t i2c_write_nbytes(uint8_t i2c_addr, uint16_t i2c_write_addr, uint8_t *i2c_write_data, uint8_t len)
{
    //For user implement
    i2c_master_write_mem16(I2C_MASTER_NUM, ND03_DEFAULT_SLAVE_ADDR, i2c_write_addr, i2c_write_data, len);
    return 0;
}


static void delay_1ms(uint32_t count)
{
    //For user implement
    ets_delay_us(1000);
}

static void delay_10us(uint32_t count)
{
    //For user implement
    ets_delay_us(10);
}


static void set_xshut_pin_level(uint32_t level)
{
    //For user implement
}

#endif /* MODULE_TOF_ENABLE */