
#include <basic.h>

/* required */
#include <inttypes.h>
#include <netinet/in.h>

/* my choice */
#include <sys/socket.h>

#define __const_htons(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8));
#define __const_ntohs __const_htons

static inline uint16_t ntohs(uint16_t x) {
        return (((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8));
}

static inline uint16_t htons(uint16_t x) {
        return (((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8));
}

static inline uint32_t ntohl(uint32_t x) {
        return (
                ((x & 0xFF000000) >> 24) |
                ((x & 0x00FF0000) >> 8)  |
                ((x & 0x0000FF00) << 8)  |
                ((x & 0x000000FF) << 24)
        );
}

static inline uint32_t htonl(uint32_t x) {
        return (
                ((x & 0xFF000000) >> 24) |
                ((x & 0x00FF0000) >> 8)  |
                ((x & 0x0000FF00) << 8)  |
                ((x & 0x000000FF) << 24)
        );
}
