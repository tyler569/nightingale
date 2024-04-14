#include <stdint.h>

uint16_t net_checksum_add(uint16_t *addr, int count, uint16_t sum) {
	uint32_t sum2 = sum;
	while (count > 1) {
		sum2 += *addr++;
		count -= 2;
	}
	if (count > 0) {
		sum2 += *(uint8_t *)addr;
	}
	while (sum2 >> 16) {
		sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	}
	return (uint16_t)sum2;
}

uint16_t net_checksum_finish(uint16_t sum) { return ~sum; }

uint16_t net_checksum_begin(uint16_t *addr, int count) {
	return net_checksum_add(addr, count, 0);
}

uint16_t net_checksum(uint16_t *addr, int count) {
	uint16_t sum = net_checksum_add(addr, count, 0);
	return net_checksum_finish(sum);
}
