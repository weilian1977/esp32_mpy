# 安装编译指南 #

## 1. ESP-IDF 依赖环境 ##
   当前验证过的环境：

   `cmake: 3.16.9`

   `python: 3.6.9`

## 2. 克隆工程 ##

   因为本工程代码，使用submodule来管理esp-idf和micropython的官方代码，因此clone 本工程时，请采用以下步骤:

   `git clone https://github.com/YanMinge/esp32_mpy`

   `cd esp32_mpy/`

   `git submodule update --init`

   如果是首次克隆，您还需要更新 esp-idf 的 submodule

   进入目录 `esp32_mpy/submodule/esp-idf`，输入命令
   `git submodule update --init`

   同样的，更新 micropython的 submodule
  
   进入目录 `esp32_mpy/submodule/micropython`，输入命令
   `git submodule update --init`
   
## 3. ESP-IDF 和 micropython 工具链安装 ##

   进入目录 `esp32_mpy/submodule/esp-idf`，输入命令

   `./install.sh`

   进入目录 `esp32_mpy/submodule/micropython`，输入命令
   
   `make -C mpy-cross`
## 4. ESP-IDF 和 micropython 对应的版本 ##

    `ESP-IDF: commit-id 000d3823bb2ff697c327958e41e9dfc6b772822a  2022.02.28`

    `micropython: commit-id 3c2aa5ff93a3b12723c7ca794aa9c583891c349b 2022.02.28`

    `esp32-camera: commit-id d1f6b9c2af104e40b461a21884ec2db90a8e2d61 2021.07.29`

    `lvgl_esp32_drivers: commit-id cccb932d3c8b0035632bfff866d242308d0ce167 2021.06.08`

    `lvgl: commit-id 65475a75ba603bf78930085e23833a04f4fc70e9 2021.07.29`

## 5. 编译工程 ##

   现在您可以从 `esp32_mpy/project/aitest` `esp32_mpy/project/oak_lite`或启动编译您的固件
   
   首先进入 `/project/aitest`或`/project/oak_lite` 文件目录

   - 执行 `. ./set_idf.sh` 使用submodule中的esp-idf环境

   - 执行 `make` 编译代码(首次编译，需要先执行make)
   
   如果编译没有出错，您在目录 `project/aitest/` 中应该可以看到新创建的 `build-GENERIC_S3`文件夹。在这个文件夹中，有编译生成的 `application.bin` 等文件。
   
   其它可能经常会用到指令包括

   - 执行 `make erase` 擦除模块的flash
   
   - 执行 `make deploy` 编译并烧录固件

   - `oak_lite`初次使用时需要打包并上传fat格式的文件系统镜像。`make make_fatfs`打包fat文件系统，`make upload_fatfs`上传fat文件系统镜像。如果挂在成功后，不需要再次打包并上传fat格式的文件系统镜像。如果更改了partitions.csv，需同步更新Makefile中`upload_fatfs`的address.

## 6. 串口工具 ##
   因为本工程固件支持repl模式，所以可以使用 pytty等串口工具来进行repl调试。
   [点击下载putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)

## 7. 连接你的设备 ##



# 开机使用 #

## 1. putty正确配置串口 ##


## 2. 使用 REPL测试结果 ##
 
# 文件结构说明 #

## 1. 主要文件结构 ##

- `doc` 文档资料
- `driver` 硬件驱动，主要是串口驱动程序
- `hardware` 开源主控的硬件设计资料
- `project` 开源主控的应用代码
- `submodule` 第三方库的内容，包括esp-idf和micropython
- `tools` 以供使用的一些工具的源码
- `List of FAQ` 常见问题的答疑


# 开发计划 #

- 支持micropython 官方的 network，usocket，urequest，uart，webrepl.