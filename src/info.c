#include "info.h"
#include "common_functions.h"


bool bit_destuff(
		uint8_t *dest, size_t dest_n,
		const uint8_t *src,  size_t src_n,
		struct bit_stuff_state *state) {
/* bit stuffs src_n*sizeof(*src) bytes from src into dest
 * returns false if an error occured 
 * returns true  if a complete frame is found 
 * caller must check state->contiguous_bit_count to check for flags
 *
 */

	state->flag_found = false;
	
	bool bit;
	while(!(
				state->get_state.src_consumed ||
				state->set_state.dest_filled  ||
				state->flag_found)) {
		bit = get_bit(src, src_n, &state->get_state);
		if(state->contiguous_bit_count >= 5) { // this is 6th bit
			if(bit) { // flag found
				state->flag_found = true;
				state->contiguous_bit_count += 1;
			}
			else { // destuff this
				state->contiguous_bit_count = 0;
			}
		}
		else {
			if(bit) {
				state->contiguous_bit_count += 1;
			}
			else {
				state->contiguous_bit_count = 0;
			}
			set_bit(dest, dest_n, bit, &state->set_state);
		}
	}
	if(state->flag_found) {
		if (get_bit(src, src_n, &state->get_state)) {
			// TODO possible segfault since not checking src_consumed
			// 7 contiguous bits
			// also sets src to bit after flag
			state->contiguous_bit_count += 1;
			return false;
		}
		else {
			return true;
		}
	}
	else {
		return false;
	}
}

