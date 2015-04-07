#include "fcs.h"
#include<stdio.h>

bool frame_check(
		const uint8_t *src, const size_t src_n,
		uint16_t *dest,
		struct frame_check_state *state,
		bool last) {
	/* Finds the fcs of src_n bytes of src and writes it to destinaton
	 * state->accumulator must be initialized with an initial value
	 * state->src_index must be set to 0 before recalling
	 * 16 0 bits must be supplied at the end of the data stream
	 * last must be set to true when the last 16 zero bits are input
	 *
	 * returns true if src is comsumed
	 * 
	 * polynomial value used is 0x1021
	 */
	// Actual divisor is 0x11021 but we match the first 1 bit which XOR
	// to 0 and are discarded.
	const uint16_t divisor = 0x1021;
	
	const unsigned int src_shift_mod         = (8 * sizeof(state->src_mask));
	const unsigned int accumulator_shift_mod = 
		(8 * sizeof(state->accumulator_mask));
	state->src_mask         = 0x1 << (src_shift_mod - 1);
	state->accumulator_mask = 0x1 << (accumulator_shift_mod - 1);

	while(state->src_index < src_n) {
		// Shift till msb == 1
		bool last_iteration; // To shift out that msb
		for(
				last_iteration = false;
				((state->accumulator & state->accumulator_mask) == 0) ||
					last_iteration; ) {
			// Shift accumulator
			state->accumulator <<= 1;
			if(
					src[state->src_index] &
					(state->src_mask >> state->shift_count)) {
				state->accumulator |= 0x1;
			}
			// maintain src masks and pointers
			state->shift_count += 1;
			if(!(state->shift_count < src_shift_mod )) {
				state->shift_count  = 0;
				state->src_index   += 1;
			}

			if(!(state->src_index < src_n)) {
				break;
			}
			if (last_iteration) {
				break; // preserve last_iteration = true
			}
		}
		if(last || last_iteration) {
			state->accumulator ^= divisor;
		}
	}
	state->src_index = 0;
	*dest = state->accumulator;
	// Src consumed
	return !last;
}


