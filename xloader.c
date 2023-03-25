/*
 *  X1000 series bootloader for u-boot/rtos/linux
 *
 *  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 *    Author: Zhang YanMing <yanming.zhang@ingenic.com, jamincheung@126.com>
 *
 *  Copyright (C) 2023 SudoMaker, Ltd.
 *    Author: Reimu NotMoe <reimu@sudomaker.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "xloader.h"


#ifdef CONFIG_BOOT_MMC
uint32_t cpu_freq = CONFIG_CPU_SEL_PLL == APLL ? CONFIG_APLL_FREQ : CONFIG_MPLL_FREQ;

#elif (defined CONFIG_BOOT_USB)
/*
 * Bootrom set apll freq for usb boot
 */
#if (CONFIG_EXTAL_FREQ == 24)
uint32_t cpu_freq = 576;
#elif (CONFIG_EXTAL_FREQ == 26)
uint32_t cpu_freq = 624;
#endif

#else
uint32_t cpu_freq = CONFIG_EXTAL_FREQ;
#endif

__attribute__((weak)) void board_init(void) {

}

__attribute__((weak)) void board_early_init(void) {

}

void x_loader_main(void) {
	check_jump_to_usb_boot();

	/*
	 * Open CPU AHB0 APB RTC TCU EFUSE NEMC clock gate
	 */
	cpm_outl(0x87fbfffc, CPM_CLKGR);

	/*
	 * Oh, ho, ho
	 * It's magic you know
	 * Never believe, it's not so
	 */
	cpm_outl(0, CPM_PSWC0ST);
	cpm_outl(16, CPM_PSWC1ST);
	cpm_outl(24, CPM_PSWC2ST);
	cpm_outl(8, CPM_PSWC3ST);


	board_early_init();

	/*
	 * Init uart
	 */
	uart_init();

	uart_puts("\nX-Loader Build: " X_LOADER_DATE " - " X_LOADER_TIME "\n");

	/*
	 * check SOC id
	 */
#ifdef CONFIG_CHECK_SOC_ID
	check_socid();
#endif

	/*
	 * Print error pc register
	 */
	uint32_t errorpc;
	__asm__ __volatile__ (
	"mfc0 %0, $30, 0\n\t"
	"nop \n\t"
	:"=r"(errorpc)
	:);
	printf("\nEPC: 0x%x\n", errorpc);

	clk_init();

	lpddr_init();

	board_init();

	xloader_storage_init();
	xloader_storage_load_stage2();

	xloader_stage2();

	hang();
}
