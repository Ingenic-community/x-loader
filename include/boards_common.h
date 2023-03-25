/*
 *  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 *
 *  X1000 series bootloader for u-boot/rtos/linux
 *
 *  Zhang YanMing <yanming.zhang@ingenic.com, jamincheung@126.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef BOARDS_COMMON_H
#define BOARDS_COMMON_H

#define __STR__(S)  #S
#define STR(S)      __STR__(S)

#define DRAM_BASE		0x80000000

#define CONFIG_SOFT_BURN
/*
 * Kernel image type
 */
#define ZIMAGE   0
#define XIMAGE   1
#define VMLINUX  2

/*
 * PM_SUSPEND_STANDBY: cpu enter idle & memory entry self-refresh
 * PM_SUSPEND_MEM:     cpu enter sleep & memory entry self-refresh & clock
 *                     all stoped & cpu power down
 */
#define PM_SUSPEND_STANDBY 2
#define PM_SUSPEND_MEM     3


#ifdef CONFIG_BOOT_UBOOT
#ifndef CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR
#define CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR 0x80100000
#endif
#ifndef CONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR
#define CONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR 0x80100000
#endif
#endif

#ifdef CONFIG_BOOT_RTOS
#ifndef CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR
#define CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR 0x80800000
#endif
#ifndef CONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR
#define CONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR 0x80800000
#endif
#endif

#ifdef CONFIG_BOOT_KERNEL
#ifndef CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR
#define CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR 0x80f00000
#endif
#ifndef CONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR
#define CONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR 0x80f00000
#endif
#endif


// ===========================================================
// The following configure could be override at BOARD.h
// ===========================================================

/*
 * External crystal freq - unit(MHz)
 */
#define CONFIG_EXTAL_FREQ           24

/*
 * PLL freq - unit(MHz)
 */
#define CONFIG_APLL_FREQ        1200
#define CONFIG_MPLL_FREQ        600
#define CONFIG_CPU_SEL_PLL      APLL
#define CONFIG_DDR_SEL_PLL      MPLL
#define CONFIG_DDR_FREQ_DIV     3
#define CONFIG_L2CACHE_CLK_DIV  2
#define CONFIG_AHB_CLK_DIV      3
#define CONFIG_PCLK_DIV         6
/*
 * DDR
 */
#define CONFIG_MDDR_H5MS5122DFR_J3M
#define CONFIG_PROBE_MEM_SIZE

#ifdef CONFIG_PROBE_MEM_SIZE
#define CONFIG_MEM_SIZE_64M
#endif

/*
 * 000: Full strength driver
 * 001: Half strength driver
 * 110: Quarter strength driver
 * 111: Octant strength driver
 * 100: Three-quarters strength driver
 */
#define CONFIG_DDR_DRIVER_STRENGTH 4

/*
 * 1: 32bits
 * 0: 1bits
 */
#define CONFIG_DDR_DW32 0

/*
 * Check efuse SoC id
 */
//#define CONFIG_CHECK_SOC_ID

/*
 * Console
 */
#define CONFIG_CONSOLE_ENABLE

/*
 * SFC
 */


/*
 * SFC freq - unit(MHz)
 */
#define  CONFIG_SFC_FREQ            (150)

#undef CONFIG_SPI_STANDARD

/*
 * Nand - bytes per-page
 */
#define CONFIG_NAND_BPP             (2048)

/*
 * Nand - page per-block
 */
#define CONFIG_NAND_PPB             (64)

/*
 * Nand - bus width
 */
#define NAND_BUSWIDTH          NAND_BUSWIDTH_8

/*
 * MMC freq - unit(MHz)
 */
#define CONFIG_MSC_FREQ             (50)

/*
 * RTC clk sel
 */
#undef CONFIG_RTCCLK_SRC_EXT

/*
 * DDR R/W test
 */
#undef CONFIG_DDR_ACCESS_TEST

/*
 * WIFI MAC
 */
#undef CONFIG_GET_WIFI_MAC
#define KERNEL_ARGS_WIFI_MAC    "wifi_mac=xxxxxxxxxxxx "

/*
 * Kernel args common
 */
#define KERNEL_ARGS_MEM         "mem=xxM@0x0 "
#define KERNEL_ARGS_OTHERS      "lpj=5009408 ip=off "
#define KERNEL_ARGS_INIT        "init=/linuxrc "

/*
 * Recovery
 */
#undef CONFIG_RECOVERY
#define RECOVERY_UPDATE_FLAG_CHECK

#define RECOVERY_UPDATE_FLAG_OFFSET             0x6000      //24kb
#define RECOVERY_UPDATE_FLAG_SIZE                   4               //unit: byte
#define RECOVERY_UPDATE_FLAG_UPDATING         0x5A5A5A5A

/*
 * Watchdog
 */
#undef CONFIG_WDT
#define CONFIG_WDT_CLK_SRC_RTC
#define CONFIG_WDT_TIMEOUT_MS 1000

/*
 * Debug power
 */
#undef CONFIG_PM_SUSPEND
#undef CONFIG_PM_SUSPEND_STATE

#endif /* BOARDS_COMMON_H */
