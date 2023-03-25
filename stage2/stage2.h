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

#pragma once

#include <common.h>

#define DRAM_BASE		0x80000000

void xloader_stage2_init();

void __attribute__((long_call)) xloader_stage2_boot_uimage(uint32_t from_storage_offset, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3);
