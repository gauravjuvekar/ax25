#pragma once
#include<inttypes.h>
#include<stdbool.h>
#include<stddef.h>
#define UINT8_T_MSB_MASK 0x80u

uint8_t byterev(uint8_t byte);

struct get_bit_state {
	uint8_t src_mask;
	size_t  src_index;
	bool    src_consumed;
};

struct set_bit_state {
	uint8_t dest_mask;
	size_t  dest_index;
	bool    dest_filled;
};

bool get_bit(const uint8_t *src, size_t src_n,  struct get_bit_state *state);
void set_bit(
		uint8_t *dest, size_t dest_n,
		bool bit,  struct set_bit_state *state);


