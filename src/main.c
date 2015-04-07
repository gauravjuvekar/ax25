#include<stdio.h>
#include "fcs.h"
#include<stdlib.h>
#include<string.h>
#include "common_functions.h"


int main(int argc, char *argv[]) {
	size_t input_n = argc - 1;
	uint8_t input[input_n];
	size_t count=0;

	for(count=0;count < input_n;count++) {
		input[count] = (uint8_t) strtol(argv[count + 1],NULL, 16);
	}


	for(count = 0 ; count < input_n ; ++count) {
		/*input[count] = byterev(input[count]);*/
		printf("%2hhx ", input[count]);
	}
	puts("\n\n");
	
	uint16_t output;
	uint8_t zero[2];
	zero[0] = zero[1] = 0;

	struct frame_check_state  state;
	state.shift_count = 0;
	state.src_mask    = 0;
	state.src_index   = 0;
	state.accumulator = 0xffff;

	frame_check(input, input_n, &output, &state, false);

	printf("fcs: %x\n", output);
	printf("shift_count:%u\n", state.shift_count);
	printf("src_index:%lu\n", state.src_index);
	printf("accum:%u\n", state.accumulator);
	puts("\n\n\n");

	state.src_index = 0;
	state.shift_count = 0;
	frame_check(zero, 2, &output, &state, true);

	printf("fcs: %x\n", output);
	printf("shift_count:%u\n", state.shift_count);
	printf("src_index:%lu\n", state.src_index);
	printf("accum:%u\n", state.accumulator);
}
