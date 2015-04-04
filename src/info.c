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
	const unsigned int src_shift_mod  = (8 * sizeof(state->src_mask ));
	const unsigned int dest_shift_mod = (8 * sizeof(state->dest_mask));

	state->src_mask  = 0x1 << (src_shift_mod  -1);
	state->dest_mask = 0x1 << (dest_shift_mod -1);
	for(; state->dest_index < dest_n; ++(state->dest_index)) {
		for(; state->dest_count < dest_shift_mod; ++(state->dest_count)) {
			if(src[state->src_index] & 
					(state->src_mask)>>(state->src_count % src_shift_mod)) {
				// Current bit is 1
				state->contiguous_bit_count += 1;
				if(state->contiguous_bit_count <= 5) {
					// Fill the src bit
					dest[state->dest_index] |=  // Fill 1
						(state->dest_mask) >>
						(state->dest_count % dest_shift_mod);
					++(state->src_count);
				}
				else {
					// Stuff 0
					state->contiguous_bit_count = 0;
					dest[state->dest_index] &= ~( // Fill 0
						(state->dest_mask) >>
						(state->dest_count % dest_shift_mod));
				}
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
		state->dest_count = 0;
	}
	// dest is filled
	state->dest_index = 0;
	return false;
}
