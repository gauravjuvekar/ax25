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
	frame->address_count = i + 1;
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

	size_t buffer_n = 1024;
	uint8_t buffer[buffer_n];
	uint8_t buffer_destuffed[buffer_n];

	struct bit_stuff_state destuff_state;
	destuff_state.dest_mask            = 0;
	destuff_state.dest_count           = 0;
	destuff_state.dest_index           = 0;
	destuff_state.contiguous_bit_count = 0;

	bool destuff_ret;

	while(!feof((FILE *)input)) {
		destuff_state.src_mask             = 0;
		destuff_state.src_count            = 0;
		destuff_state.src_index            = 0;
		fread(
				buffer,
				sizeof(buffer[0]),
				sizeof(buffer)/sizeof(buffer[0]),
				(FILE *)input
		);
		
		do {
			destuff_ret = bit_destuff(
					buffer_destuffed, buffer_n,
					buffer, buffer_n,
					&destuff_state
			);
			if(!destuff_ret) { // dest filled or flag
				if(destuff_state.contiguous_bit_count == 6) { 
					//because flag found
					//we have a frame
					size_t size = destuff_state.dest_index;
					if(size > 17) { // Probably a valid frame
						read_frame(buffer_destuffed, size, &frame);
						fwrite(
								frame.info,
								sizeof(uint8_t), frame.info_count,
								output
						);
					}
					//reset dest buffer for next frame
					destuff_state.dest_count           = 0;
					destuff_state.dest_index           = 0;
					destuff_state.contiguous_bit_count = 0;
				}
				else {
					//dest filled, shouldnt happen :TODO:
				}
			}
		} while (!destuff_ret);
	}
	free(frame.address);
	free(frame.info);
}
