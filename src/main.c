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
	state.get_state.src_mask= 0x80u;
	state.set_state.dest_mask= 0x80u;
	state.get_state.src_index  = 0;
	state.set_state.dest_index = 0;
	state.contiguous_bit_count = 0;

	bool ret;
	size_t i;

	for (i = 0 ; i < sizeof(input)/2; i++) {
	state.get_state.src_consumed = false;
	state.get_state.src_index  = 0;
	state.get_state.src_mask= 0x80u;
	ret = bit_stuff(
			output, sizeof(output)/sizeof(output[0]),
			&input[i * 2],  2,
			&state );


    /*for(count = 0; count < sizeof(output)/sizeof(output[0]); count++)*/
        /*printf("%02x", output[count]);*/
    /*printf("\n--------------\n");*/
	
	
	}


    for(count = 0; count < sizeof(input)/sizeof(input[0]); count++)
        printf("%02x", input[count]);
    printf("\n--------------\n");

    for(count = 0; count < sizeof(output)/sizeof(output[0]); count++)
        printf("%02x", output[count]);
    printf("\n--------------\n");
	printf("ret                        = %d\n",  ret);
	printf("state.get.cons             = %d\n",  state.get_state.src_consumed);
	printf("state.set.fill             = %d\n",  state.set_state.dest_filled);
	printf("state.src_count            = %d\n",  state.get_state.src_mask);
	printf("state.dest_count           = %d\n",  state.set_state.dest_mask);
	printf("state.src_index            = %zu\n", state.get_state.src_index);
	printf("state.dest_index           = %zu\n", state.set_state.dest_index);
	printf("state.contiguous_bit_count = %d\n",  state.contiguous_bit_count);

    return(0);
}
