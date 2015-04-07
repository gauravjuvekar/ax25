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
	
	const unsigned int src_shift_mod   = (8 * sizeof(state->src_mask));
	const uint32_t polynomial_msb_mask = 0x1 << (17 - 1);
	state->src_mask                    = 0x1 << (src_shift_mod - 1);

	while(state->src_index < src_n) {
		// Shift till msb == 1
		while((state->accumulator & polynomial_msb_mask) == 0) {
			// Shift accumulator
			state->accumulator <<= 1;
			// Fill accumulator lsb
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
		}
		if ((state->accumulator & polynomial_msb_mask) || last) {
			state->accumulator ^= divisor;
		}
	}
	state->src_index = 0;
	*dest = (uint16_t)((state->accumulator) & (0xffff));
	// Src consumed
	return !last;
}


