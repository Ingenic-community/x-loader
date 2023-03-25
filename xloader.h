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

#pragma once

#include <common.h>

void xloader_storage_init(void);
int xloader_storage_load(uint32_t offset, uint32_t length, void *load_addr);
void xloader_storage_load_stage2();

void __attribute__((long_call)) xloader_stage2();
