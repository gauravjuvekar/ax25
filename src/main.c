#include<stdio.h>
#include "fcs.h"
#include<stdlib.h>
#include<string.h>
#include "common_functions.h"


int main(int argc, char *argv[]) {
	/*const char *pos = argv[1];*/
	size_t input_n = strlen(argv[1]);

	size_t i;
	for(i = 0 ; i < input_n ; ++i) {
		argv[1][i] = byterev(argv[1][i]);
		printf("%2hhx ", argv[1][i]);
	}
	puts("\n\n");
	
	uint8_t input[2];
	input[0] = input[1] = 0;
	uint16_t output;
	/*size_t count=0;*/
	/*for(count=0;count < input_n;count++) {*/
		/*sscanf(pos, "%2hhx", &input[count]);*/
		/*pos += 2*sizeof(input[0]);*/
	/*}*/

	struct frame_check_state  state;
	state.shift_count = 0;
	state.mask        = 0;
	state.src_index   = 0;
	state.accumulator = 0x0000;

	frame_check((uint8_t *)argv[1], input_n, &output, &state);

	printf("fcs: %x\n", output);
	printf("shift_count:%u\n", state.shift_count);
	printf("src_index:%lu\n", state.src_index);
	printf("accum:%u\n", state.accumulator);
	puts("\n\n\n");

	frame_check(input, 2, &output, &state);

	printf("fcs: %x\n", output);
	printf("shift_count:%u\n", state.shift_count);
	printf("src_index:%lu\n", state.src_index);
	printf("accum:%u\n", state.accumulator);
}
