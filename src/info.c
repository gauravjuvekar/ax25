#include "info.h"


bool bit_destuff(
		uint8_t *dest, size_t dest_n,
		const uint8_t *src,  size_t src_n,
		struct bit_stuff_state *state) {
	/* bit stuffs src_n*sizeof(*src) bytes from src into dest
	 * returns false if dest is filled or flag is found
	 * returns true  if src is consumed
	 * caller must check state->contiguous_bit_count to check for flags
	 *
	 */
	const unsigned int src_shift_mod  = (8 * sizeof(state->src_mask ));
	const unsigned int dest_shift_mod = (8 * sizeof(state->dest_mask));

	state->src_mask  = 0x1 << (src_shift_mod  -1);
	state->dest_mask = 0x1 << (dest_shift_mod -1);
	for(; state->dest_index < dest_n; ++(state->dest_index)) {
		for(
				state->dest_count = 0;
				state->dest_count < dest_shift_mod;
				++state->dest_count) {

			if(state->contiguous_bit_count == 5) {
				if(
						src[state->src_index] & 
						(state->src_mask)>>(state->src_count % src_shift_mod)) {
					//flag detected
					++state->contiguous_bit_count;
					// Point to next src
					state->src_count += 2;
					if(!(state->src_count < src_shift_mod)) {
						state->src_count = 0;
						++(state->src_index);
					}
					return false; 
					// caller should check state->contiguous_bit_count
					// == 6 to check if a flag is detected
				}
				else {
					//destuff
					++state->src_count;
					if(!(state->src_count < src_shift_mod)) {
						state->src_count = 0;
						++(state->src_index);
						if(!(state->src_index < src_n)) { // src is consumed
							state->src_index = 0;
							return true; 				
						}
					}
					continue;
				}
			}
			if(src[state->src_index] & 
					(state->src_mask)>>(state->src_count % src_shift_mod)) {
				// Current bit is 1
				state->contiguous_bit_count += 1;
				dest[state->dest_index] |=  // Fill 1
					(state->dest_mask) >>
					(state->dest_count % dest_shift_mod);
				++(state->src_count);
			}
			else {
				// Current bit is 0
				state->contiguous_bit_count = 0;
				dest[state->dest_index] &= ~( // Fill 0
						(state->dest_mask) >>
						(state->dest_count % dest_shift_mod));
				++(state->src_count);
			}

			if(!(state->src_count < src_shift_mod)) {
				state->src_count = 0;
				++(state->src_index);
				if(!(state->src_index < src_n)) { // src is consumed
					state->src_index = 0;
					return true; 				
				}
			}
		}
	}
	// dest is filled
	// state->dest_index points to the next byte
	return false;
}

