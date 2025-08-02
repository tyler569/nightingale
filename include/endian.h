#pragma once

/*
 * endian.h - byte order definitions for Nightingale OS
 */

#include <stdint.h>

/* Define byte order constants */
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 4321
#define PDP_ENDIAN 3412

/* Define the byte order for this system (x86_64 is little endian) */
#ifdef __LITTLE_ENDIAN__
#define BYTE_ORDER LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__)
#define BYTE_ORDER BIG_ENDIAN
#else
/* Default to little endian for x86_64 */
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* Byte swapping functions */
static inline uint16_t bswap16(uint16_t x) {
	return ((x & 0xff) << 8) | ((x & 0xff00) >> 8);
}

static inline uint32_t bswap32(uint32_t x) {
	return ((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8)
		| ((x & 0xff000000) >> 24);
}

static inline uint64_t bswap64(uint64_t x) {
	return ((uint64_t)bswap32(x & 0xffffffff) << 32) | bswap32(x >> 32);
}

/* Host to network and network to host conversions */
#if BYTE_ORDER == LITTLE_ENDIAN
#define htobe16(x) bswap16(x)
#define htole16(x) (x)
#define be16toh(x) bswap16(x)
#define le16toh(x) (x)

#define htobe32(x) bswap32(x)
#define htole32(x) (x)
#define be32toh(x) bswap32(x)
#define le32toh(x) (x)

#define htobe64(x) bswap64(x)
#define htole64(x) (x)
#define be64toh(x) bswap64(x)
#define le64toh(x) (x)
#else
#define htobe16(x) (x)
#define htole16(x) bswap16(x)
#define be16toh(x) (x)
#define le16toh(x) bswap16(x)

#define htobe32(x) (x)
#define htole32(x) bswap32(x)
#define be32toh(x) (x)
#define le32toh(x) bswap32(x)

#define htobe64(x) (x)
#define htole64(x) bswap64(x)
#define be64toh(x) (x)
#define le64toh(x) bswap64(x)
#endif