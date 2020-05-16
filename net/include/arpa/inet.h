
#include <basic.h>

/* required */
#include <inttypes.h>
#include <netinet/in.h>

/* my choice */
#include <sys/socket.h>

#define __const_htons(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8));
#define __const_ntohs __const_htons

#define ntohl(x) (\
    (((x) & 0xFF000000) >> 24) | \
    (((x) & 0x00FF0000) >> 8 ) | \
    (((x) & 0x0000FF00) << 8 ) | \
    (((x) & 0x000000FF) << 24))
#define htonl(x) ntohl(x)

static inline uint16_t ntohs(be16 x) {
        return (((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8));
}

static inline be16 htons(uint16_t x) {
        return (((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8));
}

