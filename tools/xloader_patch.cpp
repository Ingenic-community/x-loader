//
// Created by root on 2023/3/3.
//
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>

#include <cstdio>
#include <cinttypes>
#include <cstring>
#include <cassert>

#include "../lib/tiny-AES-c/aes.h"

static const uint8_t crc7_syndrome_table[256] = {
	0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
	0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
	0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
	0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
	0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
	0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
	0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
	0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
	0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
	0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
	0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
	0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
	0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
	0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
	0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
	0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
	0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
	0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
	0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
	0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
	0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
	0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
	0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
	0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
	0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
	0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
	0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
	0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
	0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
	0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
	0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
	0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};

static inline uint8_t crc7_byte(uint8_t crc, uint8_t data) {
	return crc7_syndrome_table[(crc << 1) ^ data];
}

static uint8_t crc7(uint8_t crc, uint8_t *buffer, size_t len) {
	while (len--)
		crc = crc7_byte(crc, *buffer++);
	return crc;
}

static inline uint32_t crc32_byte(uint32_t r, uint8_t data) {
	r ^= data;

	for (int i = 0; i < 8; i++) {
		uint32_t t = ~((r&1) - 1);
		r = (r>>1) ^ (0xEDB88320 & t);
	}

	return r;
}

static const uint8_t stage2_key_placeholder[32] = {
	0xef, 0xef, 0xef, 0xef, 0x5A, 0xF8, 0x61, 0x00,
	0x31, 0x7F, 0xA1, 0x80, 0xfe, 0xfe, 0xfe, 0xfe,
	0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe, 0xba, 0xbe,
	0xaa, 0x55, 0x55, 0xaa, 0xef, 0xef, 0xef, 0xef
};

static const uint8_t stage2_salt_placeholder[16] = {
	0xca, 0xfe, 0xbe, 0xef, 0xef, 0xef, 0xfe, 0xfe,
	0xba, 0xbe, 0xfa, 0xce, 0x5A, 0xF8, 0x61, 0x00
};

static void show_help() {
	puts("Usage: xloader_patch <stage1.bin> <stage2.bin>\n"
	     "\n"
	     "This utility patches stage1 binary ");
}

static std::vector<uint8_t> read_file(const char *path) {
	std::ifstream fs(path, std::ios::in | std::ios::binary);
	if (!fs) {
		throw std::runtime_error(std::string("failed to open file ") + path);
	}
	std::vector<uint8_t> ret((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
	return ret;
}

static void write_file(const char *path, const std::vector<uint8_t> &data) {
	std::ofstream fs(path, std::ios::out | std::ios::trunc | std::ios::binary);
	if (!fs) {
		throw std::runtime_error(std::string("failed to open file ") + path);
	}
	fs.write(reinterpret_cast<const char *>(data.data()), (long)data.size());
	fs.close();
	if (!fs.good()) {
		throw std::runtime_error(std::string("failed to write file ") + path);
	}
}

#define SPL_HEADER_SIZE			2048
#define SPL_CRC_OFFSET			9
#define SPL_LENGTH_OFFSET		12

int main(int argc, char **argv) {

	if (argc != 3) {
		show_help();
		exit(1);
	}

	uint8_t _sbox[256], _rsbox[256];
	AES_init_sbox_rsbox(_sbox, _rsbox);

	puts("-- Reading files");

	auto stage1 = read_file(argv[1]);
	auto stage2 = read_file(argv[2]);

	std::random_device rdev;
	std::mt19937 rng(rdev());
	std::uniform_int_distribution<std::mt19937::result_type> udist(0,255);

	std::vector<uint8_t> new_key(sizeof(stage2_key_placeholder));
	std::vector<uint8_t> new_salt(sizeof(stage2_salt_placeholder));

	puts("-- Patching stage1");

	if (stage1.size() < SPL_HEADER_SIZE) {
		puts("Error: Stage1 smaller than SPL header, this is impossible.");
		exit(2);
	}

	auto it_key = std::search(stage1.begin(), stage1.end(), stage2_key_placeholder, stage2_key_placeholder + sizeof(stage2_key_placeholder));
	if (it_key == stage1.end()) {
		puts("Error: Stage2 key placeholder not found in stage1. It's probably already patched.");
		exit(1);
	} else {
		auto off_key = std::distance(stage1.begin(), it_key);
		printf("Info: Stage2 key placeholder found at offset 0x%lx in stage1\n", off_key);
		printf("Info: New key: ");
		for (auto &it : new_key) {
			it = udist(rng);
			printf("%02x", it);
		}
		printf("\n");

		memcpy(&stage1[off_key], new_key.data(), sizeof(stage2_key_placeholder));
	}

	auto it_salt = std::search(stage1.begin(), stage1.end(), stage2_salt_placeholder, stage2_salt_placeholder + sizeof(stage2_salt_placeholder));
	if (it_salt == stage1.end()) {
		puts("Error: Stage2 salt placeholder not found in stage1. It's probably already patched.");
		exit(1);
	} else {
		auto off_salt = std::distance(stage1.begin(), it_salt);
		printf("Info: Stage2 salt placeholder found at offset 0x%lx in stage1\n", off_salt);
		printf("Info: New salt: ");
		for (auto &it : new_salt) {
			it = udist(rng);
			printf("%02x", it);
		}
		printf("\n");

		memcpy(&stage1[off_salt], new_salt.data(), sizeof(stage2_salt_placeholder));
	}

	uint8_t stage1_crc7 = 0;
	uint32_t stage1_len = stage1.size();

	for (unsigned i=SPL_HEADER_SIZE; i<stage1.size(); i++) {
		stage1_crc7 = crc7_byte(stage1_crc7, stage1[i]);
	}

	memcpy(&stage1[SPL_LENGTH_OFFSET], &stage1_len, sizeof(uint32_t));
	stage1[SPL_CRC_OFFSET] = stage1_crc7;

	printf("Info: Stage1 CRC7 checksum: 0x%02x\n", stage1_crc7);
	printf("Info: Stage1 total length: 0x%02x\n", stage1_len);

	puts("-- Patching stage2");

	{
		unsigned pad_len = AES_BLOCKLEN - (stage2.size() % AES_BLOCKLEN);
		printf("Info: Stage2 is %zu bytes, need to pad %u bytes\n", stage2.size(), pad_len);

		for (unsigned i = 0; i < pad_len; i++) {
			stage2.push_back(udist(rng));
		}
	}

	std::vector<uint8_t> stage2_encrypted;

	// Encrypted stage2 layout:
	// | CRC32x4 (16 bytes) | IV (16 bytes) | Encrypted code (16 * n bytes) |

	// This is awfully like homemade crypto, but we're short on SRAM space.
	// So we can't use a proper hash function even like MD5.
	// But it's better than nothing.
	// https://crypto.stackexchange.com/questions/87665/is-there-any-hash-collision-attack-that-is-not-defeated-by-adding-a-salt-first

	const size_t stage2_crc_len = 16;

	stage2_encrypted.resize(stage2_crc_len);

	{
		printf("Info: IV: ");
		for (unsigned i = 0; i < AES_BLOCKLEN; i++) {
			uint8_t ivb = udist(rng);
			printf("%02x", ivb);
			stage2_encrypted.push_back(ivb);
		}
		printf("\n");

		stage2_encrypted.insert(stage2_encrypted.end(), stage2.begin(), stage2.end());

		AES_ctx aes_ctx{};
		AES_init_ctx_iv(&aes_ctx, new_key.data(), stage2_encrypted.data() + stage2_crc_len);
		AES_CBC_encrypt_buffer(&aes_ctx, stage2_encrypted.data() + stage2_crc_len + AES_BLOCKLEN, stage2.size());
	}

	{
		size_t stage2_partition_size = 0x3000;

		if (stage2_encrypted.size() > stage2_partition_size) {
			printf("Error: Stage2Encrypted larger than partition size (%zu > %zu)\n", stage2_encrypted.size(), stage2_partition_size);
			exit(2);
		}

		unsigned pad_len = stage2_partition_size - stage2_encrypted.size();
		printf("Info: Stage2Encrypted is %zu bytes, padding to partition size (%zu bytes)\n", stage2_encrypted.size(), stage2_partition_size);

		for (unsigned i = 0; i < pad_len; i++) {
			stage2_encrypted.push_back(udist(rng));
		}
	}

	{
		uint32_t stage2_code_crc[4];
		assert(stage2_crc_len == sizeof(stage2_code_crc));

		printf("Info: Stage2 CRC after encryption: ");
		for (unsigned i = 0; i < 4; i++) {
			uint32_t t = ~0;

			for (unsigned j = 0; j < 4; j++) {
				t = crc32_byte(t, new_salt[i * 4 + j]);
			}

			for (unsigned j=stage2_crc_len; j<stage2_encrypted.size(); j++) {
				t = crc32_byte(t, stage2_encrypted[j]);
			}

			stage2_code_crc[i] = ~t;
			printf("%08x ", stage2_code_crc[i]);
		}
		printf("\n");

		memcpy(stage2_encrypted.data(), stage2_code_crc, sizeof(stage2_code_crc));
	}


	puts("-- Writing files");

	write_file(argv[1], stage1);
	write_file(argv[2], stage2_encrypted);

	puts("-- Finished. Have a good day.");

	return 0;

}