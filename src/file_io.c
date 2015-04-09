#include "file_io.h"
#include<stdlib.h>
#include<stdio.h>
#include "frame.h"
#include<sysexits.h>
#include "addressing.h"
#include "pid.h"
#include "control.h"
#include "fcs.h"
#include<arpa/inet.h>
#include "common_functions.h"
#include "info.h"
#include<string.h>
#include "specifications.h"


void read_frame(
		uint8_t buffer[], size_t buffer_n,
		struct frame *frame) {
	size_t i = 0;
	for(i = 0; i < buffer_n - 2; ++i) {
		buffer[i] = byterev(buffer[i]);
	}
	// Write address
	for(i = 0; ; ++i) { 
		frame->address[i] = buffer[i];
		if(buffer[i] & 1) {
			break;
		}
	}
	frame->address_count = (i + 1) / 7;
	++i;
	frame->control       = buffer[i];
	++i;
	frame->pid           = buffer[i];
	++i;
	// info
	frame->info_count = buffer_n - 2 - i;
	memcpy(frame->info, &buffer[i], frame->info_count);
	//fcs
	memcpy(&frame->fcs, &buffer[buffer_n-2],2);
	frame->fcs = ntohs(frame->fcs); // Endianness
}


void decode_main_loop(const FILE *input, FILE *output) {

	struct frame frame;
	frame.address       = malloc(sizeof(*frame.address)*7*ROUTING_MAX_NODES);
	frame.info          = malloc(sizeof(*frame.info)*512);

	if(frame.address == NULL || frame.info == NULL) {
		fprintf(stderr, "fatal: malloc failed\n");
		free(frame.address);
		free(frame.info);
		fclose((FILE *)input); // Deconstantify
		fclose(output);
		exit(EX_OSERR);
	}



	const size_t buffer_n = 1024;
	uint8_t buffer[buffer_n];
	uint8_t buffer_destuffed[buffer_n];

	struct bit_destuff_state destuff_state;
	bool destuff_ret;

	destuff_state.contiguous_bit_count = 0;
	destuff_state.flag_found = false;
	destuff_state.data_error = false;

	while(!feof((FILE *)input)) {
		// New src buffer
		destuff_state.get_state.src_mask     = UINT8_T_MSB_MASK;
		destuff_state.get_state.src_index    = 0;
		destuff_state.get_state.src_consumed = false;
		fread(
				buffer,
				sizeof(buffer[0]), sizeof(buffer)/sizeof(buffer[0]),
				(FILE *)input
		);
		
		do {
			// Get interflag data destuffed into dest

			destuff_state.set_state.dest_mask   = UINT8_T_MSB_MASK;
			destuff_state.set_state.dest_index  = 0;
			destuff_state.set_state.dest_filled = false;

			destuff_ret = bit_destuff(
					buffer_destuffed, buffer_n,
					buffer,           buffer_n,
					&destuff_state
			);

			if(destuff_ret) { //got a frame 
				if(destuff_state.flag_found) { 
					// we have a frame
					// ignore flag which is the last byte
					// destuff_state.set_state.dest_index is set to the next
					// byte
					if (destuff_state.set_state.dest_index == 0) {
						continue;
						/*size_t underflows*/
					}
					size_t size = destuff_state.set_state.dest_index - 1;

					if(size >= 17) { 
						// Probably a valid frame
						read_frame(buffer_destuffed, size, &frame);
						fwrite(
								frame.info,
								sizeof(uint8_t), frame.info_count,
								output
						);
					}
					else {
						// got interspace between two flags
					}
				}
				else if (destuff_state.data_error) {
					fprintf(stderr,
							"Data error, 7 contiguous bits detected.\n");

					free(frame.address);
					free(frame.info);
					exit(EX_DATAERR);
				}
			}
		} while (!destuff_state.get_state.src_consumed);
	}

	free(frame.address);
	free(frame.info);
}
