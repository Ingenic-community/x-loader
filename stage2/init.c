/*
  This file is part of x-loader.

  Copyright (C) 2023 SudoMaker
  Author: Reimu NotMoe <reimu@sudomaker.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  You should have received a copy of the GNU General Public License along
  with this program; if not, see <https://www.gnu.org/licenses/>.
*/

#include "stage2.h"

#include "lib/lwmem/lwmem.h"

static const lwmem_region_t lwmem_regions[3] = {
	// DRAM: 128KB - 1MB
	{ (void *)(DRAM_BASE + 0x20000), 0xE0000 },
	// DRAM: 6MB to infinity
	{ (void *)(DRAM_BASE + 0x500000), 77108864 },
	{ NULL, 0 }
};

void xloader_stage2_init() {
	if (!lwmem_assignmem(lwmem_regions))
		hang_reason("Cannot initialize LwMEM");

	printf("stage2 inited\n");
}

extern void board_boot(void);

void xloader_stage2() {
	xloader_stage2_init();
	board_boot();
	hang();
}
