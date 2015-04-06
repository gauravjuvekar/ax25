#include "fcs.h"

void frame_check(
		const uint8_t *src, const size_t src_n,
		uint16_t *dest,
		struct frame_check_state *state) {
	/* Finds the fcs of src_n bytes of src and writes it to destinaton
	 * state->accumulator must be initialized with an initial value
	 * state->src_index must be set to 0 before recalling
	 * if src is changed
	 * 16 0 bits must be supplied at the end of the data stream
	 * 
	 * polynomial value used is 0x1021
	 */
	// Actual divisor is 0x11021 but we match the first 1 bit which XOR
	// to 0 and are discarded.
	const uint16_t divisior = 0x1021;
	
	const unsigned int shift_mod = (8 * sizeof(state->mask));
	state->mask = 0x1 << (shift_mod - 1);

	while(state->src_index < src_n) {
		while((state->accumulator & state->mask) == 0) {
			state->accumulator <<= 1;
			if(src[state->src_index] & (state->mask >> state->shift_count)) {
				state->accumulator |= 0x1;
			}
			state->shift_count += 1;
			if(!(state->shift_count < shift_mod )) {
				state->shift_count  = 0;
				state->src_index   += 1;
			}
		}
		// Shift out the MSB 1 since it's going to be zeroed.
		state->accumulator <<= 1;
		if(src[state->src_index] & (state->mask >> state->shift_count)) {
			state->accumulator |= 0x1;
		}
		state->shift_count += 1;
		if(!(state->shift_count < shift_mod )) {
			state->shift_count  = 0;
			state->src_index   += 1;
		}
		state->accumulator ^= divisior;
	}
	*dest = state->accumulator;
}


