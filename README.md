# x-loader

Minimalistic bootloader for Ingenic SoCs.

Only the X1000 series (X1000(E), X1500, X1501) is supported at this moment.

## Note
This project is under construction.

## FAQ
### 如何编译？


### 如何添加新板极？


### 如何引导RTOS？


### 如何引导zImage？


### 如何引导vmlinux？

kernel编译: make vmlinux.bin

### 板极配置文件在哪里？

### 如何针对板子做特定初始化？

board.c中board_init()用于在引导前对板子
进行初始化，当然也可以没有

### 如何修改Soc各时钟频率？

include/configs/boards_common.h为默认值
打开板极头文件BOARD.h，去定义默认值再重新定义期望值

### 如何修改串口号以及波特率？

打开include/configs/BOARD_NAME.h
修改：
```c
#define CONFIG_CONSOLE_BAUDRATE      3000000
#define CONFIG_CONSOLE_PC
#define CONFIG_CONSOLE_INDEX         2
```

### 如何关闭系统串口打印？

打开include/configs/BOARD_NAME.h
```c
#undef CONFIG_CONSOLE_ENABLE
```

### 如何添加自定义的kernel cmdline？
```c
打开include/configs/BOARD_NAME.h
#define KERNEL_ARGS_BOARD "xxx "
```


### 系统没有32KHz晶振如何休眠唤醒？

打开include/configs/BOARD_NAME.h
增加定义：#define CONFIG_RTCCLK_SRC_EXT

### 如何添加recovery引导功能？

打开include/configs/BOARD_NAME.h
增加定义：#define CONFIG_RECOVERY

### 如何使能watch dog功能？

打开include/configs/BOARD_NAME.h
增加定义：#define CONFIG_WDT
默认timeout为1000ms
通过#define CONFIG_WDT_TIMEOUT_MS修改默认值

### x-loader中如何进入低功耗状态？
打开include/configs/BOARD_NAME.h
增加定义：#define CONFIG_PM_SUSPEND
以及
```c
#define CONFIG_PM_SUSPEND_STATE PM_SUSPEND_STANDY ---> CPU进入idle状态
#define CONFIG_PM_SUSPEND_STATE PM_SYSPEND_MEM ---> CPU进入sleep状态
```

在boards/BOARD_NAME/board.c中实现gpio_ss_table[][2]数组设置休眠状态，可以不实现

### x-loader中如何进入USB烧录模式？
打开include/configs/BOARD_NAME.h
增加定义：#define CONFIG_SOFT_BURN
在适当位置调用set_jump_to_usbboot();机器会重启进入烧录模式

### 烧录什么文件？
编译完成会有提示：“Image: xxxxxx” is ready

### 添加了代码编译不过怎么办？
首先确认代码没有超过12KByte，如果没有继续debug

### 引导不成功怎么办？

## History
This is a fork of the [original x-loader](https://github.com/JaminCheung/x-loader) written by *Yanming Zhang* from Ingenic.

## License
GPLv2 or later
