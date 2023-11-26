#include <stdint.h>

template <class next_header>
class ethernet_frame {
public:
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
    next_header payload;
};

template <class next_header>
class ipv4_packet {
public:
    uint8_t version_ihl;
    uint8_t dscp_ecn;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
    next_header payload;
};