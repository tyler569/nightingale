#pragma once

#include <stdint.h>

struct framebuffer {
	uint32_t width, height, pitch, bpp;
	void *address;
};

struct framebuffer get_framebuffer();
