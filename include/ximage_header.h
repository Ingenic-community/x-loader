#pragma once

#include "types.h"

struct ximage_image_desc {
	uint32_t type;
	uint32_t flags;
	uint32_t length;
	uint32_t load_addr;
};

struct ximage_header {
	uint32_t version;
	uint32_t flags;
	uint32_t image_count;
};

