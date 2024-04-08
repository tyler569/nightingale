#pragma once

#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

static inline uint32_t htonl(uint32_t hostlong) {
	return ((hostlong & 0xff) << 24) | ((hostlong & 0xff00) << 8)
		| ((hostlong & 0xff0000) >> 8) | ((hostlong & 0xff000000) >> 24);
}

static inline uint16_t htons(uint16_t hostshort) {
	return ((hostshort & 0xff) << 8) | ((hostshort & 0xff00) >> 8);
}

static inline uint32_t ntohl(uint32_t netlong) {
	return ((netlong & 0xff) << 24) | ((netlong & 0xff00) << 8)
		| ((netlong & 0xff0000) >> 8) | ((netlong & 0xff000000) >> 24);
}

static inline uint16_t ntohs(uint16_t netshort) {
	return ((netshort & 0xff) << 8) | ((netshort & 0xff00) >> 8);
}

END_DECLS

