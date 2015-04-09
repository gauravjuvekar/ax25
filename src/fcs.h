#pragma once
#include<inttypes.h>
#include<stdbool.h>
#include<stddef.h>

struct frame_check_state {
	unsigned int shift_count;
	uint8_t  src_mask;
	size_t   src_index;
	uint32_t accumulator;
};

bool frame_check(
		const uint8_t *src, const size_t src_n,
		uint16_t *dest,
		struct frame_check_state *state,
		bool last
);
