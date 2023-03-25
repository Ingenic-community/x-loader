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

#include "lib/tiny-AES-c/aes.h"

static const uint8_t stage2_key[32] = {
	0xef, 0xef, 0xef, 0xef, 0x5A, 0xF8, 0x61, 0x00,
	0x31, 0x7F, 0xA1, 0x80, 0xfe, 0xfe, 0xfe, 0xfe,
	0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe, 0xba, 0xbe,
	0xaa, 0x55, 0x55, 0xaa, 0xef, 0xef, 0xef, 0xef
};

static const uint8_t stage2_salt[16] = {
	0xca, 0xfe, 0xbe, 0xef, 0xef, 0xef, 0xfe, 0xfe,
	0xba, 0xbe, 0xfa, 0xce, 0x5A, 0xF8, 0x61, 0x00
};

void xloader_storage_init(void) {
	int rc;
#ifdef CONFIG_BOOT_MMC
	rc = mmc_init();
#endif

#ifdef CONFIG_BOOT_SFC
	sfc_init();
#endif

#ifdef CONFIG_BOOT_SPI_NAND
	rc = spinand_init();
#endif

#ifdef CONFIG_BOOT_SPI_NOR
	rc = spinor_init();
#endif

	if (rc)
		hang_reason("storage_init failed\n");
}

int xloader_storage_load(uint32_t offset, uint32_t length, void *load_addr) {
#if defined(CONFIG_BOOT_MMC)
	return mmc_read(offset, length, load_addr);
#elif defined(CONFIG_BOOT_SPI_NAND)
	return spinand_read(offset, length, load_addr);
#elif defined(CONFIG_BOOT_SPI_NOR)
	return spinor_read(offset, length, (uint32_t)load_addr);
#else
	#error No boot media defined!
#endif
}

static uint32_t crc32_byte(uint32_t r, uint8_t data) {
	r ^= data;

	for (int i = 0; i < 8; i++) {
		uint32_t t = ~((r&1) - 1);
		r = (r>>1) ^ (0xEDB88320 & t);
	}

	return r;
}

void xloader_storage_load_stage2() {
	int rc = xloader_storage_load(CONFIG_STAGE2_OFFSET, CONFIG_STAGE2_LENGTH, (void *)DRAM_BASE);
	if (rc)
		hang_reason("stage2 load failed");

	uint32_t *crc = (uint32_t *)DRAM_BASE;

	for (unsigned i=0; i<4; i++) {
		uint32_t t = ~0;

		for (unsigned j=0; j<4; j++) {
			t = crc32_byte(t, stage2_salt[i * 4 + j]);
		}

		for (unsigned j=16; j<CONFIG_STAGE2_LENGTH; j++) {
			t = crc32_byte(t, ((uint8_t *)DRAM_BASE)[j]);
		}

		if (~t != crc[i]) {
			hang_reason("stage2 damaged");
		}
	}

	uint8_t sbox[256], rsbox[256];
	AES_init_sbox_rsbox(sbox, rsbox);

	struct AES_ctx aes_ctx;
	AES_init_ctx_iv(&aes_ctx, stage2_key, (void *)(DRAM_BASE + 16));
	AES_CBC_decrypt_buffer(&aes_ctx, (void *)(DRAM_BASE + 32), 12256);

	flush_cache_all();
}

