#include "fcs.h"
#include<stdio.h>
#include<string.h>

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}
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
	const uint16_t divisor = 0x1021;
	
	const unsigned int src_shift_mod         = (8 * sizeof(state->src_mask));
	const unsigned int accumulator_shift_mod = (8 * sizeof(state->accumulator_mask));
	state->src_mask         = 0x1 << (src_shift_mod - 1);
	state->accumulator_mask = 0x1 << (accumulator_shift_mod - 1);

	while(state->src_index < src_n) {
		while((state->accumulator & state->accumulator_mask) == 0) {


			state->accumulator <<= 1;

			if(src[state->src_index] & (state->src_mask >> state->shift_count)) {
				state->accumulator |= 0x01;
			}
			state->shift_count += 1;
			if(!(state->shift_count < src_shift_mod )) {
				state->shift_count  = 0;
				state->src_index   += 1;
			}

			printf("Accumulator:");
			printf("%s ",byte_to_binary(((uint8_t*)&(state->accumulator))[1]));
			printf("%s\n",byte_to_binary(((uint8_t*)&(state->accumulator))[0]));
		}
		// Shift out the MSB 1 since it's going to be zeroed.
		state->accumulator <<= 1;
		if(src[state->src_index] & (state->src_mask >> state->shift_count)) {
			state->accumulator |= 0x01;
		}
		state->shift_count += 1;
		if(!(state->shift_count < src_shift_mod )) {
			state->shift_count  = 0;
			state->src_index   += 1;
		}

		printf("MSB_shift:\n");
		printf("Accumulator:");
		printf("%s ",byte_to_binary(((uint8_t*)&(state->accumulator))[1]));
		printf("%s XOR \n",byte_to_binary(((uint8_t*)&(state->accumulator))[0]));

		printf("Divisor    :");
		printf("%s ",byte_to_binary(((uint8_t*)&(divisor))[1]));
		printf("%s =\n",byte_to_binary(((uint8_t*)&(divisor))[0]));

		state->accumulator ^= divisor;
		printf("Accumulator:");
		printf("%s ",byte_to_binary(((uint8_t*)&(state->accumulator))[1]));
		printf("%s \n",byte_to_binary(((uint8_t*)&(state->accumulator))[0]));
		printf("---------------------------------\n");
	}
	*dest = state->accumulator;
}


