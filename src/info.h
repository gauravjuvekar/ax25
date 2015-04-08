#pragma once
#include<inttypes.h>
#include<stddef.h>
#include<stdbool.h>

struct bit_stuff_state {
	uint8_t src_mask;
	unsigned int src_count;

	uint8_t dest_mask;
	unsigned int dest_count;

	size_t src_index;
	size_t dest_index;

	unsigned int contiguous_bit_count;
};


bool bit_destuff(
		uint8_t *dest, size_t dest_n,
		const uint8_t *src,  size_t src_n,
		struct bit_stuff_state *state
);
