#include <ng/limine.h>
#include <ng/panic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TEXT_SCALE 1

uint32_t *fb;
uint32_t width, height;
void video_print(uint32_t x, uint32_t y, const char *str) {
	extern unsigned char font8x8_basic[128][8];

	for (size_t i = 0; i < strlen(str); i++) {
		for (size_t j = 0; j < 8 * TEXT_SCALE; j++) {
			for (size_t k = 0; k < 8 * TEXT_SCALE; k++) {
				if (font8x8_basic[(unsigned char)str[i]][j / TEXT_SCALE]
						& (1 << (k / TEXT_SCALE)))
					fb[(y + j) * width + i * 8 * TEXT_SCALE + x + k]
						= 0xffffffff;
			}
		}
	}
}

void video_scroll(uint32_t lines) {
	lines *= 8 * TEXT_SCALE;
	for (size_t i = 0; i < height - lines; i++) {
		memcpy(fb + i * width, fb + (i + lines) * width, width * 4);
	}
	for (size_t i = height - lines; i < height; i++) {
		memset(fb + i * width, 0, width * 4);
	}
}

void video() {
	uint32_t pitch, bpp;
	void *address;
	limine_framebuffer(&width, &height, &bpp, &pitch, &address);

	printf("framebuffer: %ux%u, %u bpp, pitch %u, address %p\n", width,
			height, bpp, pitch, address);

	if (bpp != 32)
		panic("framebuffer bpp is not 32");

	fb = (uint32_t *)address;

	video_print(0, 0, "Hello world!");
	video_print(0, 16, "abcdefghijklmnopqrstuvwxyz");
	video_print(0, 32, "0123456789");
	video_print(0, 48, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	video_print(0, 64, "`~!@#$%^&*(){}[]_+-=:;\"'<>,.?/\\");
	video_print(0, 80, "video_print(128, 128, \"Hello world!\");");

	int y = 96;

	char buf[64];
	for (int i = 0; i < 30; i++) {
		snprintf(buf, 64, "Hello world! %i", i);
		video_print(0, y, buf);
		y += 8 * TEXT_SCALE;
		if (y >= height) {
			y -= 8 * TEXT_SCALE;
			video_scroll(1);
		}
	}
}
