#pragma once

#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct udp_hdr {
	uint16_t src_port;
	uint16_t dest_port;
	uint16_t length;
	uint16_t checksum;
};

END_DECLS
