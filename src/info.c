#include "info.h"


bool bit_stuff(
		uint8_t *dest, size_t dest_n,
		const uint8_t *src,  size_t src_n,
		struct bit_stuff_state *state) {
	/* bit stuffs src_n*sizeof(*src) bytes from src into dest
	 * returns false if dest is filled
	 * returns true  if src  is consumed
	 *
	 */
	bool bit;
	
	while(!(
				state->get_state.src_consumed ||
				state->set_state.dest_filled)) {
		if(state->contiguous_bit_count >= 5) {
			// stuff 0
			set_bit(dest, dest_n, false, &state->set_state);
			state->contiguous_bit_count = 0;
		}
		else {
			bit = get_bit(src, src_n, &state->get_state);
			set_bit(dest, dest_n, bit, &state->set_state);
			if(bit) {
				state->contiguous_bit_count += 1;
			}
			else {
				state->contiguous_bit_count = 0;
			}
		}
	}
	if(state->get_state.src_consumed) {
		return true;
	}
	else {
		return false;
	}
}
