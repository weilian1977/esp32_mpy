# ND03开发指南

## 示例代码目录介绍

示例代码目录结构如下，代码中包含了设备参数设置、深度获取、非线性标定、串扰标定等功能。

```c
|--ND03_SDK
    |-- inc 头文件
    	|-- nd03_calib.h
    	|-- nd03_comm.h
    	|-- nd03_data.h
    	|-- nd03_def.h
    	|-- nd03_dev.h
    |-- src 源码
    	|-- nd03_calib.c:	ND03标定函数源文件
    	|-- nd03_comm.c:	ND03通讯接口源文件
    	|-- nd03_data.c:	ND03测量接口源文件
    	|-- nd03_dev.c:		ND03设备配置源文件
   	|-- example： ND03获取深度数据的例程.
                  |-- ND03_Ranging_Example.c： 一个简单实现的获取深度数据的例程.
                	|-- nd03_platform.c:  用户需要根据使用的平台进行实现的接口.
                  |-- nd03_platform.h  
                    
```



## I2C通讯

- **默认设备地址**：

```c
0x5B
```

- **修改I2C地址方式**：

 ```c
ND03支持修改i2c设备地址，但由于模组没有对修改后的地址进行保存，重新上电则自动恢复默认i2c地址0x5B.
修改ND03设备的i2c地址后，切记本地同时要更新新的地址.
接口如下：
/*! 
 * @brief 设置ND03的设备地址
 * @param pMyDevice: ND03模组设备信息结构体指针
 * @param addr: 配置ND03模组的i2c设备地址
 * @return int32_t
 */
int32_t ND03_SetSlaveAddr(ND03_Dev_t *pMyDevice, uint8_t addr);
 
//TODO 更新本地对ND03通讯的slave地址
 ```


## ND03初始化配置

- **配置库函数**

  ND03库预留出来4个接口需要用户实现并且注册到SDK中；SetXShutPinLevelFunc为设置XShut电平函数指针，参数值为0，则输出低电平，否则输出高电平需要用户实现并赋值。

  Note:其中*I2C_WriteNBytesFunc*和*I2C_ReadNBytesFunc*为可以指定读写长度的i2c连续读写函数。

```c
typedef struct{
	int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t); /*!< i2c写函数 */
	int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t); /*!< i2c读函数 */
	void(*Delay1msFunc)(uint32_t );   /*!< 延时1ms函数 */
	void(*Delay10usFunc)(uint32_t );  ///< 延时10us函数
}ND03_Func_Ptr_t;

/** 模组设备 */
ND03_Dev_t g_nd03_device = {.i2c_dev_addr = ND03_DEFAULT_SLAVE_ADDR, .SetXShutPinLevelFunc = set_xshut_pin_level};
	
    /* 函数指针结构体 */
    ND03_Func_Ptr_t dev_op = {NULL, NULL, NULL, NULL}; 

    /* 初始化函数指针结构体 */
    dev_op.Delay10usFunc = delay_10us;
    dev_op.Delay1msFunc = delay_1ms;
    dev_op.I2C_ReadNBytesFunc = i2c_read_nbytes;
    dev_op.I2C_WriteNBytesFunc = i2c_write_nbytes;

    /* 将host端功能函数注册到SDK中 */
    ND03_RegisteredPlatformApi(dev_op);
```

- **ND03启动**

  初始化ND03之前，首先需要调用ND03_WaitDeviceBootUp()函数启动模组；

```c
  /**
   * @brief ND03 Wait for Device Boot Up (等待ND03模组启动)
   *
   * @param   pMyDevice   模组设备
   * @return  ND03_ERROR_NONE:成功
   * @return  ND03_ERROR_BOOT:启动失败--请检测模组是否焊接好，还有i2c地址与读写函数是否错误。
   * @return  ND03_ERROR_FW:固件不兼容--请与FAE联系，是否模组的固件与SDK不兼容。
   */
   int32_t ND03_WaitDeviceBootUp(ND03_Dev_t *pMyDevice);

   /* 循环等待设备启动, 若模组或者IIC读写函数有问题则会报错 */
   if(ND03_ERROR_NONE != ND03_WaitDeviceBootUp(&g_nd03_device))
   {
       NX_PRINTF("ND03_WaitDeviceBootUp error\r\n");
       return -1;	
   }
```
​	     如下图ND03_WaitDeviceBootUp()启动函数分为两阶段：

​			第一阶段：*ND03_REG_BOOT1~ND03_REG_BOOT5*寄存器配置启动值；

​			第二阶段：判断ND03是否已经进入工作状态，ND03工作状态寄存器地址为ND03_REG_STATE(*0xA7A4*),读取到0xA5或者0xA6则说明ND03工作正常。

```c
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
        }while((pNxDevice->dev_pwr_state != ND03_STATE_SOFTWARE_READY) && (pNxDevice->dev_pwr_state != ND03_STATE_GOT_DEPTH)  \   				&& --try_times);

        if((ND03_ERROR_NONE != ret) || (0 == try_times))
        {
            NX_PRINTF("ND03 boot error\r\n");
            return ND03_ERROR_BOOT;
        }

        NX_PRINTF("%s End!\r\n", __func__);
        return ND03_ERROR_NONE;
    }
```

- **ND03初始化**

```c
    /**
     * @brief ND03 InitDevice(初始化设备)
     *
     * @param   pMyDevice   模组设备
     * @return  int32_t
     */
	int32_t ND03_InitDevice(ND03_Dev_t *pMyDevice);

	/* 初始化模组设备 */
	if(ND03_ERROR_NONE != ND03_InitDevice(&g_nd03_device))
	{
		NX_PRINTF("ND03_InitDevice error!!\r\n");
		return -1;
	}
```

## ND03工作模式配置与测量

- **ND03工作模式配置**

|       Range Mode       |            Device Mode             |
| :--------------------: | :--------------------------------: |
| ND03_RANGE_MODE_NORMAL |   ND03_DEVICEMODE_SINGLE_RANGING   |
|  ND03_RANGE_MODE_FAST  | ND03_DEVICEMODE_CONTINUOUS_RANGING |

如上表格，ND03每一个工作模式由四个配置字组合而成，用户可根据需要使用ND03_SetRangeMode进行配置：

   - ND03_RANGE_MODE_NORMAL：普通模式，测量帧率比快速模式低，但是精度比快速模式要高，均方差比快速低。
   - ND03_RANGE_MODE_FAST：快速模式，测量帧率根据不同测量深度范围在44~89 fps。
   - ND03_DEVICEMODE_SINGLE_RANGING：单次测量模式，上位机对ND03进行一次测量触发，并且读取数据。
   - ND03_DEVICEMODE_CONTINUOUS_RANGING：连续测量模式，上位机对ND03进行一次触发测量，ND03会不断的进入测量模式，上位机需要不断等待数据有效并且进行读取。

  Note:模组出厂默认为普通 & 单次，如需更改模式需要使用ND03_SetRangeMode进行配置。

**Example**:

```c
	//设置快速模式
	ND03_SetRangeMode(&g_nd03_device, ND03_RANGE_MODE_FAST | (ND03_GetRangeMode(&g_nd03_device) & (~ND03_RANGE_MODE_MASKc)));
	
	//设置连续模式
	ND03_SetRangeMode(&g_nd03_device, ND03_DEVICEMODE_CONTINUOUS_RANGING | (ND03_GetRangeMode(&g_nd03_device)             \
    & (~ND03_DEVICEMODE_MASKc)));
```

具体例程可以参考/src/example/中的示例代码.

- **测量**

  - 单次工作模式：

  在单次工作模式下，运行ND03_StartMeasurement后，ND03会触发一次测量，通过ND03_GetRangingData来获取深度幅度数据。

  ```c
      ND03_RangingData_t data;
  	
      //设置ND03取图模式单次普通取图模式
      ND03_SetRangeMode(&g_nd03_device, ND03_DEVICEMODE_SINGLE_RANGING | ND03_RANGE_MODE_NORMAL);
  	
  	/* 请求一次测量 */
      ND03_StartMeasurement(&g_nd03_device);
  
      /* 获取测量数据 */
      ND03_GetRangingData(&g_nd03_device, &data);
  
      NX_PRINTF("depth:%d mm, amp:%d, exp:%d\r\n", data.depth, data.amp, ND03_GetCurrentExp(&g_nd03_device));
  ```

  - 连续工作模式

  在连续工作模式下，运行ND03_StartMeasurement后，ND03会一直触发测量，循环调用ND03_GetRangingData来获取深度幅度数据，调用ND03_StopMeasurement来停止ND03的测量任务。

  ```c
      ND03_RangingData_t data;
      uint16_t measure_cnt = 100;
  
      //设置ND03取图模式连续普通取图模式
      ND03_SetRangeMode(&g_nd03_device, ND03_DEVICEMODE_CONTINUOUS_RANGING | ND03_RANGE_MODE_NORMAL);
  
      /* 开始测量 */
      ND03_StartMeasurement(&g_nd03_device);
  
      while(measure_cnt--)
    	{
          /* 获取测量数据 */
          ND03_GetRangingData(&g_nd03_device, &data);
          NVprintf("depth:%d mm, amp:%d, exp:%d\r\n", data.depth, data.amp, ND03_GetCurrentExp(&g_nd03_device));
      }
  
      /* 结束测量模式 */
      ND03_StopMeasurement(&g_nd03_device);
  ```

## 非线性标定

模组出厂内部已经标定好非线性标定数据，不过用户使用时工作电压与出厂的电压（3.3V）有差异的话，需要进行重新标定，否则精度会降低。

在非线性标定的环境下(模组放在距90%反射板15cm处)，调用ND03_Calibration接口，标定完成后标定数据会保存在模组内部flash中，开机自动读取flash中的标定数据，故标定一次后如果精度符合要求即可反复断上电使用。

```c
/**
 * @brief ToF非线性标定
 * 
 * @param   pMyDevice   设备模组
 * @return  int32_t  
 * @retval  0:  成功
 * @retval  17: 数据非线性错误
 * @retval  18: 数据抖动错误
 */
int32_t ND03_Calibration(ND03_Dev_t *pMyDevice);
```

## 玻璃盖板（串扰）

如果用户有在ND03表面加装玻璃盖板的需求，则需要使用到串扰标定和矫正的代码。

- **串扰标定**

在串扰标定的环境下(模组加盖板放在距18%反射板80cm处)，调用ND03_XTalkCalibration接口，标定完成后标定数据会保存在模组内部flash中，开机自动读取flash中的标定数据，故标定一次后如果精度符合要求即可反复断上电使用。

```c
/**
 * @brief 串扰标定功能
 * 
 * @note  矫正玻璃盖板串扰
 * 
 * @param   pMyDevice   模组设备       
 * @return  int32_t   
 * @retval  0:  成功
 * @retval  !0: 串扰标定失败
 */
int32_t ND03_XTalkCalibration(ND03_Dev_t *pMyDevice);
```

## GPIO引脚功能

TBD



## 低功耗

### 1.进入低功耗模式

#### （1）软件进入

​	TBD

#### （2）硬件进入

​	给一个下降沿到ND03模组xshut引脚，即可进入低功耗模式，可通过调用ND03_HardwareSleep()实现。

```c
/**
 * @brief 硬件待机api
 * @param   pMyDevice   模组设备
 * @return  void  
 */
void ND03_HardwareSleep(ND03_Dev_t *pMyDevice);
```

### 2.唤醒低功耗模式

给一个上升沿到xshut引脚，即可对ND03模组进行唤醒操作，可通过ND03_HardwareWakeup()实现。

```c
/**
 * @brief 从硬件待机中唤醒api
 * @param   pMyDevice   模组设备
 * @return  void  
 */
void ND03_HardwareWakeup(ND03_Dev_t *pMyDevice);
```

**Note**：使用ND03_HardwareSleep()与ND03_HardwareWakeup()接口前需要用户实现可对xshut脚设置高低电平的接口，按照函数指针 void(*SetXShutPinLevelFunc)(uint32_t )的格式进行定义并在初始化阶段赋值给它即可。
