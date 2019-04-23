
#pragma once
#ifndef NIGHTINGALE_NET_INET_H
#define NIGHTINGALE_NET_INET_H

#include <ng/basic.h>
#include <stdint.h>

static inline uint16_t htons(uint16_t s) {
        return (s & 0xFF00) >> 8 | (s & 0x00FF) << 8;
}

static inline uint16_t ntohs(uint16_t s) {
        return (s & 0xFF00) >> 8 | (s & 0x00FF) << 8;
}

static inline uint32_t htonl(uint32_t s) {
        return (s & 0xFF000000) >> 24 | (s & 0x00FF0000) >> 8 |
               (s & 0x0000FF00) << 8 | (s & 0x000000FF) << 24;
}

static inline uint32_t ntohl(uint32_t s) {
        return (s & 0xFF000000) >> 24 | (s & 0x00FF0000) >> 8 |
               (s & 0x0000FF00) << 8 | (s & 0x000000FF) << 24;
}

#endif
