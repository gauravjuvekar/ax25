#pragma once
#include<inttypes.h>
#include<stddef.h>
#include<stdbool.h>
#include "common_functions.h"

struct bit_destuff_state {
	struct get_bit_state get_state;
	struct set_bit_state set_state;

	bool flag_found;
	bool data_error;
	unsigned int contiguous_bit_count;
};


bool bit_destuff(
		uint8_t *dest, size_t dest_n,
		const uint8_t *src,  size_t src_n,
		struct bit_destuff_state *state
);
