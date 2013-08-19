#ifndef _PPERF_H_
#define _PPERF_H_
#include <stdint.h>

struct test_header {
	uint16_t 	ether_proto;
	uint16_t	__PAD;
	uint32_t	ul_proto;
	char		test_name[64];
};

#endif
