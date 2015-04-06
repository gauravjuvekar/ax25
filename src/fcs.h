#pragma once
#include<inttypes.h>
#include<stdbool.h>
#include<stddef.h>

struct frame_check_state {
	unsigned int shift_count;
	uint8_t mask;
	size_t src_index;
	uint16_t accumulator;
};

void frame_check(
		const uint8_t *src, const size_t src_n,
		uint16_t *dest,
		struct frame_check_state *state
);
