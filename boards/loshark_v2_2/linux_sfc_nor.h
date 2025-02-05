/*
  Board configuration file for SudoMaker LoShark

  Copyright (C) 2023 SudoMaker
  Author: ReimuNotMoe <reimu@sudomaker.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  You should have received a copy of the GNU General Public License along
  with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

// Tab is 8 spaces

#include <lpddr/chip/LPDDR_PMD606416A_8M.h>

/*
 * Console
 */
#define CONFIG_UART_INDEX			2
#define CONFIG_UART_BAUDRATE			115200
#define CONFIG_UART_REG_DIV			0xd
#define CONFIG_UART_REG_UMR			0x10
#define CONFIG_UART_REG_UACR			0x0

#define CONFIG_CONSOLE_PA

#define CONFIG_RTCCLK_SRC_EXT

#define CONFIG_BOOT_KERNEL
#define CONFIG_BOOT_SFC
#define CONFIG_BOOT_SPI_NOR
#define CONFIG_KERNEL_IMAGE_TYPE ZIMAGE


#define CONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR	0x80350000
#define CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR	0x80350000

#define WDT_CFG_TCSR          0x2
#define WDT_CFG_TDR           0x8001


/*
 * The following configure only for boot kernel
 */
#ifdef CONFIG_BOOT_KERNEL

#define KERNEL_ARGS_BOARD   " "

#ifdef CONFIG_CONSOLE_ENABLE
#define KERNEL_ARGS_CONSOLE     "no_console_suspend console=ttyS"STR(CONFIG_CONSOLE_INDEX)","STR(CONFIG_CONSOLE_BAUDRATE)"n8 "
#else
#define KERNEL_ARGS_CONSOLE     "console=null "
#endif

#ifdef CONFIG_GET_WIFI_MAC
#define KERNEL_ARGS_COMMON KERNEL_ARGS_MEM KERNEL_ARGS_CONSOLE KERNEL_ARGS_OTHERS KERNEL_ARGS_WIFI_MAC KERNEL_ARGS_BOARD
#else
#define KERNEL_ARGS_COMMON KERNEL_ARGS_MEM KERNEL_ARGS_CONSOLE KERNEL_ARGS_OTHERS KERNEL_ARGS_BOARD
#endif

#ifdef CONFIG_RECOVERY
#define CONFIG_RECOVERY_BOOT_KEY            GPIO_PA(10)
#define CONFIG_RECOVERY_BOOT_KEY_ENLEVEL    0
#define CONFIG_RECOVERY_ARGS KERNEL_ARGS_COMMON
#endif /* CONFIG_RECOVERY */

#endif /* CONFIG_BOOT_KERNEL */

/*
 * The following configure only for NAND boot
 */
#ifdef CONFIG_BOOT_SPI_NAND

#ifdef CONFIG_RECOVERY
#define CONFIG_KERNEL_ARGS KERNEL_ARGS_COMMON KERNEL_ARGS_INIT "ubi.mtd=4 root=ubi0:rootfs ubi.mtd=5 rootfstype=ubifs rw"
#else
#define CONFIG_KERNEL_ARGS KERNEL_ARGS_COMMON KERNEL_ARGS_INIT "ubi.mtd=3 root=ubi0:rootfs ubi.mtd=4 rootfstype=ubifs rw"
#endif /* CONFIG_RECOVERY */

/*
 * unit(byte)
 */
#define CONFIG_UBOOT_OFFSET         0x6800
#define CONFIG_UBOOT_LENGTH         0x40000

#define CONFIG_RTOS_OFFSET          0x40000
#define CONFIG_RTOS_LENGTH          0x40000

#ifdef CONFIG_KERNEL_IMAGE_TYPE
#if (CONFIG_KERNEL_IMAGE_TYPE == ZIMAGE || CONFIG_KERNEL_IMAGE_TYPE == XIMAGE)

#define CONFIG_KERNEL_OFFSET        0x100000
#define CONFIG_KERNEL_LENGTH        0x800000

#elif (CONFIG_KERNEL_IMAGE_TYPE == VMLINUX)

#define CONFIG_KERNEL_OFFSET        0x100000
#define CONFIG_KERNEL_LENGTH        0x800000

#endif
#endif

#define CONFIG_RECOVERY_OFFSET      0x980000
#define CONFIG_RECOVERY_LENGTH      0x800000

#define CONFIG_WIFI_MAC_ADDR        0x901000

#endif /* CONFIG_BOOT_SPI_NAND */

/*
 * The following configure only for NOR boot
 */


#define CONFIG_KERNEL_ARGS "console=ttyS2,115200n8 mem=8M@0x0 loglevel=7 ingenic_pm_clk=0 devtmpfs.mount=1 init=/linuxrc root=/dev/mmcblk1p1 ro"

/*
 * unit(byte)
 */

#define CONFIG_STAGE2_OFFSET        0x4000
#define CONFIG_STAGE2_LENGTH        0x3000

#define CONFIG_KERNEL_OFFSET        0x8000
#define CONFIG_KERNEL_LENGTH        0x100000


/*
 * The following configure only for MMC boot
 */
#ifdef CONFIG_BOOT_MMC

#define CONFIG_MMC_PA_4BIT
#undef CONFIG_MMC_PA_8BIT
#undef CONFIG_MMC_PC_4BIT

#define CONFIG_KERNEL_ARGS KERNEL_ARGS_COMMON KERNEL_ARGS_INIT "rootfstype=ext4 root=/dev/mmcblk0p3 rw"

/*
 * unit(byte)
 */
#define CONFIG_UBOOT_OFFSET         0xA400
#define CONFIG_UBOOT_LENGTH         0x40000

#define CONFIG_RTOS_OFFSET          0x40000
#define CONFIG_RTOS_LENGTH          0x40000

#ifdef CONFIG_KERNEL_IMAGE_TYPE
#if (CONFIG_KERNEL_IMAGE_TYPE == ZIMAGE || CONFIG_KERNEL_IMAGE_TYPE == XIMAGE)

#define CONFIG_KERNEL_OFFSET        0x300000
#define CONFIG_KERNEL_LENGTH        0x800000

#elif (CONFIG_KERNEL_IMAGE_TYPE == VMLINUX)

#define CONFIG_KERNEL_OFFSET        0x100000
#define CONFIG_KERNEL_LENGTH        0x800000

#endif
#endif

#define CONFIG_RECOVERY_OFFSET      0x400000
#define CONFIG_RECOVERY_LENGTH      0x300000

#define CONFIG_WIFI_MAC_ADDR        0x39000

#endif /* CONFIG_BOOT_MMC */

