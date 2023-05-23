/*
  Board configuration file for SudoMaker LoShark

  Copyright (C) 2023 SudoMaker
  Author: Reimu NotMoe <reimu@sudomaker.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  You should have received a copy of the GNU General Public License along
  with this program; if not, see <https://www.gnu.org/licenses/>.
*/

#include <common.h>
#include <stage2/stage2.h>

void board_early_init(void) {
	ddr_autosr = 1;
	gpio_enable_pull(GPIO_PA(2));
}

void board_init(void) {
	rtc_clk_src_to_ext();
	gpio_direction_output(GPIO_PC(26), 0);
	gpio_direction_output(GPIO_PA(15), 0);
}

void board_boot(void) {
	xloader_stage2_boot_uimage(CONFIG_KERNEL_OFFSET, 0, 0, 0, 0);
}
