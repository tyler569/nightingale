#include <ng/limine.h>
#include <ng/panic.h>
#include <stdint.h>
#include <stdio.h>
#include <stream.h>
#include <string.h>

#define TEXT_SCALE 1

extern unsigned char font8x8_basic[128][8];
extern unsigned char font_moderndos_8x16[256][16];

uint32_t *fb;
uint32_t width, height;
uint32_t cursor_x, cursor_y;

#define FONT font_moderndos_8x16
#define FONT_HEIGHT 16
#define FONT_WIDTH 8

void video_putchar(uint32_t x, uint32_t y, unsigned char c) {
	uint32_t *pixel = fb + y * width + x;
	uint8_t *glyph = FONT[c];
	for (size_t i = 0; i < FONT_HEIGHT; i++) {
		for (size_t j = 0; j < FONT_WIDTH; j++) {
			if (glyph[i] & (1 << (FONT_WIDTH - j - 1))) {
				pixel[j] = 0xffe0e0e0;
			}
		}
		pixel += width;
	}
}

void video_scroll(uint32_t lines) {
	lines *= FONT_HEIGHT * TEXT_SCALE;
	for (size_t i = 0; i < height - lines; i++) {
		memcpy(fb + i * width, fb + (i + lines) * width, width * 4);
	}
	for (size_t i = height - lines; i < height; i++) {
		memset(fb + i * width, 0, width * 4);
	}
}

ssize_t video_write(struct stream *s, const void *data, size_t count) {
	(void)s;
	const unsigned char *buf = data;

	if (!fb)
		return count;

	for (size_t i = 0; i < count; i++) {
		if (buf[i] == '\n') {
			cursor_x = 0;
			cursor_y += FONT_HEIGHT * TEXT_SCALE;
			if (cursor_y >= height) {
				cursor_y -= FONT_HEIGHT * TEXT_SCALE;
				video_scroll(1);
			}
		} else {
			video_putchar(cursor_x, cursor_y, buf[i]);
			cursor_x += FONT_WIDTH * TEXT_SCALE;
			if (cursor_x >= width) {
				cursor_x = 0;
				cursor_y += FONT_HEIGHT * TEXT_SCALE;
				if (cursor_y >= height) {
					cursor_y -= FONT_HEIGHT * TEXT_SCALE;
					video_scroll(1);
				}
			}
		}
	}
	return count;
}

struct stream_vtbl video_stream_vtbl = {
	.write = video_write,
};

struct stream *video_stream = &(struct stream) {
	.vtbl = &video_stream_vtbl,
};

void video() {
	uint32_t pitch, bpp;
	void *address;
	limine_framebuffer(&width, &height, &bpp, &pitch, &address);

	printf("framebuffer: %ux%u, %u bpp, pitch %u, address %p\n", width, height,
		bpp, pitch, address);

	if (bpp != 32)
		panic("framebuffer bpp is not 32");

	fb = (uint32_t *)address;

	fprintf(video_stream, "STREAM - Hello world!\n");
	fprintf(video_stream, "STREAM - abcdefghijklmnopqrstuvwxyz\n");
	fprintf(video_stream, "STREAM - 0123456789\n");
	fprintf(video_stream, "STREAM - ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
	fprintf(video_stream, "STREAM - `~!@#$%%^&*(){}[]_+-=:;\"'<>,.?/\\\n");

	for (int i = 0; i < 128; i++) {
		fprintf(video_stream, "STREAM - %c\n", i);
	}
}
