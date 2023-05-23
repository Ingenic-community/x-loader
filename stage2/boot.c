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

#include "../xloader.h"

#include "lib/lwmem/lwmem.h"
#include "lib/lzma/LzmaDec.h"

int __attribute__((long_call)) xloader_storage_load(uint32_t offset, uint32_t length, void *load_addr);

void *malloc(size_t size) {
	return lwmem_malloc(size);
}

void free(void *ptr) {
	lwmem_free(ptr);
}

void *lzma_Alloc(void *p, size_t size) {
	return malloc(size);
}

void lzma_Free(void *p, void *address) {
	free(address);
}

#define S2_LZMA_LOAD_BUF_SIZE	16384

static int xloader_stage2_lzma_decompress(uint32 from_storage_addr, void *to_mem_addr, uint32_t compressed_len) {
	uint8_t *src_buf = malloc(S2_LZMA_LOAD_BUF_SIZE);
	uint32 src_pos = 0;
	uint32_t src_left = compressed_len;
	uint32_t src_load_len;

	uint32_t dst_len = 6 * 1024 * 1024;
	uint8 *dst_buf = (uint8 *)to_mem_addr;
	uint32 dst_pos = 0;
	uint32_t dst_left = dst_len;

	CLzmaDec *dec = malloc(sizeof(CLzmaDec));
	LzmaDec_Construct(dec);

	ISzAlloc SzAllocForLzma = {
		.Alloc = lzma_Alloc,
		.Free = lzma_Free
	};

	SRes res;

	// 1. Load and decode props
	{
		xloader_storage_load(from_storage_addr + src_pos, LZMA_PROPS_SIZE, src_buf);
		src_pos += 13; // FDP
		src_left -= 13;


		res = LzmaDec_Allocate(dec, src_buf, LZMA_PROPS_SIZE, &SzAllocForLzma);

//		printf("res: %d\n", res);

		if (res) {
			printf("LzmaDec_Alloc failed: res=%d", res);
			return res;
		}
	}

//	printf("props ok\n");

	LzmaDec_Init(dec);

	// 2. Actual decompression

	ELzmaStatus status;

	while (src_left) {
		src_load_len = src_left > S2_LZMA_LOAD_BUF_SIZE ? S2_LZMA_LOAD_BUF_SIZE : src_left;
//		printf("load %d bytes from %x\n", src_load_len, from_storage_addr + src_pos);
		xloader_storage_load(from_storage_addr + src_pos, src_load_len, src_buf);

		src_pos += src_load_len;
		src_left -= src_load_len;

		uint32_t dst_dec_len = dst_len;

		res = LzmaDec_DecodeToBuf(dec, &dst_buf[dst_pos], &dst_dec_len,
					  src_buf, &src_load_len, LZMA_FINISH_ANY, &status);
//		printf("res: %d\n", res);

		dst_pos += dst_dec_len;
		dst_left -= dst_dec_len;

//		printf("src: %d/%d, dst: %d/%d\n", src_pos, compressed_len, dst_pos, dst_len);

//		printf("-- %x\n", *(uint32_t *)DRAM_BASE);

		if (res) {
			printf("LzmaDec failed: res=%d, status=%d", res, status);
			return res;
		}

//		puts(".");

		if (status == LZMA_STATUS_FINISHED_WITH_MARK)
			break;
	}

	LzmaDec_Free(dec, &SzAllocForLzma);

//	puts("\n");

	return 0;
}

struct uimage_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		ih_name[32];	/* Image Name			*/
};

void *memcpy_from_back(void *dest, const void *src, size_t n) {
	char *d = dest;
	const char *s = src;

	// Start from the end of the array and copy one byte at a time
	for (size_t i = n; i > 0; i--) {
		d[i-1] = s[i-1];
	}

	return dest;
}

void xloader_stage2_boot_uimage(uint32_t from_storage_offset, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
	int rc = 0;
	struct uimage_header uimage_hdr;

	xloader_storage_load(from_storage_offset, sizeof(struct uimage_header), &uimage_hdr);

	if (uimage_hdr.ih_magic != 0x56190527) {
		puts("uImage magic incorrect");
		return;
	}

	// We're little endian, aren't we?
	uimage_hdr.ih_size = __builtin_bswap32(uimage_hdr.ih_size);
	uimage_hdr.ih_load = __builtin_bswap32(uimage_hdr.ih_load);
	uimage_hdr.ih_ep = __builtin_bswap32(uimage_hdr.ih_ep);

	printf("uImage name: %s\n", uimage_hdr.ih_name);
	printf("uImage size: %d\n", uimage_hdr.ih_size);
	printf("uImage load addr: %x\n", uimage_hdr.ih_load);
	printf("uImage endpoint addr: %x\n", uimage_hdr.ih_ep);

	const void *bounce = (const void *)(DRAM_BASE + 0x20000);

	switch (uimage_hdr.ih_comp) {
		case 0:
			rc = xloader_storage_load(from_storage_offset + sizeof(struct uimage_header), uimage_hdr.ih_size, (void *) uimage_hdr.ih_load);
			break;
		case 3:
			rc = xloader_stage2_lzma_decompress(from_storage_offset + sizeof(struct uimage_header),
							    (void *)bounce, uimage_hdr.ih_size);
			memcpy_from_back((void *)uimage_hdr.ih_load, bounce, 0x400000);
			break;
		default:
			printf("uImage compression unsupported: %u\n", uimage_hdr.ih_comp);
			return;
	}

	if (rc)
		return;

	typedef void (*image_entry_t)(uint32_t, uint32_t, uint32_t, uint32_t)
		__attribute__ ((noreturn));

	image_entry_t image_entry = (image_entry_t) uimage_hdr.ih_ep;

	flush_cache_all();

	printf("Jumping to %p\n", image_entry);

	image_entry(arg0, arg1, arg2, arg3);
}
