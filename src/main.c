#include <stdio.h>
#include<stdlib.h>
#include<inttypes.h>
#include<stdbool.h>
#include"info.h"


int main(int argc, char *argv[])
{
	const char *pos = argv[3];
    uint8_t input[atoi(argv[1])];
    uint8_t output[atoi(argv[2])];
    size_t  count = 0;

     /* WARNING: no sanitization or error-checking whatsoever */
    for(count = 0; count < sizeof(input)/sizeof(input[0]); count++) {
        sscanf(pos, "%2hhx", &input[count]);
        pos += 2 * sizeof(uint8_t);
    }

	struct bit_stuff_state state;
	state.src_count  = 0;
	state.dest_count = 0;
	state.src_index  = 0;
	state.dest_index = 0;
	state.contiguous_bit_count = 0;

	bool ret = bit_stuff(
			output, sizeof(output)/sizeof(output[0]),
			input,  sizeof(input)/sizeof(input[0]),
			&state );
	
    for(count = 0; count < sizeof(input)/sizeof(input[0]); count++)
        printf("%02x", input[count]);
    printf("\n--------------\n");

    for(count = 0; count < sizeof(output)/sizeof(output[0]); count++)
        printf("%02x", output[count]);
    printf("\n--------------\n");
	printf("ret                        = %d\n",  ret);
	printf("state.src_count            = %d\n",  state.src_count);
	printf("state.dest_count           = %d\n",  state.dest_count);
	printf("state.src_index            = %zu\n", state.src_index);
	printf("state.dest_index           = %zu\n", state.dest_index);
	printf("state.contiguous_bit_count = %d\n",  state.contiguous_bit_count);

    return(0);
}
