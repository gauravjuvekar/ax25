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



unsigned int write_frame(
		unsigned int bit_rotate,
		struct frame* frame,
		FILE *output) {
	size_t i;
	/* Move bit_rotate bits into the previous byte
	 *
	 */

	// Byterev stuff
	/*for(i = 0 ; i < frame->address_count * 7; ++i) {
		frame->address[i] = byterev(frame->address[i]);
	}*/
	frame->control = byterev(frame->control);
	frame->pid     = byterev(frame->pid);
	for(i = 0 ; i < frame->info_count; ++i) {
		frame->info[i] = byterev(frame->info[i]);
	}

	// bit_stuff and write
	uint8_t buffer[1024];
	struct bit_stuff_state stuff_state;
	stuff_state.src_mask             = 0;
	stuff_state.src_count            = 0;
	stuff_state.dest_mask            = 0;
	stuff_state.dest_count           = bit_rotate;
	stuff_state.src_index            = 0;
	stuff_state.dest_index           = 0;
	stuff_state.contiguous_bit_count = 0;

	buffer[0] = 0x7eu >> ((8 - bit_rotate) % 8); // flag
	buffer[1] = 0x7eu << bit_rotate;
	bit_stuff(
			&buffer[1], 1023,
			frame->address, 7 * frame->address_count,
			&stuff_state);
	bit_stuff(
			&buffer[1], 1023,
			&frame->control, 1,
			&stuff_state);
	bit_stuff(
			&buffer[1], 1023,
			&frame->pid, 1,
			&stuff_state);
	bit_stuff(
			&buffer[1], 1023,
			frame->info, frame->info_count,
			&stuff_state);
	bit_stuff(
			&buffer[1], 1023,
			(uint8_t *)&frame->fcs, 2,
			&stuff_state);

	buffer[stuff_state.dest_index-1] |= 0x7eu >> (stuff_state.dest_count);
	buffer[stuff_state.dest_index]    = 0x7eu << (8 - stuff_state.dest_count); // flag
	fseek(output, -1, SEEK_CUR);
	uint8_t byte_buffer = 0;
	fread(&byte_buffer, 1, 1, output);
	fseek(output, -1, SEEK_CUR);
	buffer[0] |= byte_buffer;
	fwrite(buffer, 1, stuff_state.dest_index + 1, output);

	return (8 - stuff_state.dest_count);
}




void encode_main_loop(
		const FILE *input, FILE *output,
		size_t info_count,
		const char *addresses[], size_t address_count) {

	struct frame frame;

	frame.address_count = address_count;
	frame.info_count    = info_count;
	frame.address       = malloc(sizeof(*frame.address)*7*address_count);
	frame.info          = malloc(sizeof(*frame.info)*info_count); 

	if(frame.address == NULL || frame.info == NULL) {
		fprintf(stderr, "fatal: malloc failed\n");
		free(frame.address);
		free(frame.info);
		fclose((FILE *)input); // Deconstantify
		fclose(output);
		exit(EX_OSERR);
	}

	address_field(&frame, addresses, address_count);

	size_t send_sequence = 0;


	struct frame_check_state fcs_state_address; // will be constant
	// preserve this state
	fcs_state_address.accumulator = 0xffff;
	fcs_state_address.shift_count = 0;
	fcs_state_address.src_mask    = 0;
	fcs_state_address.src_index   = 0;

	frame_check(
			frame.address, frame.address_count,
			&frame.fcs,
			&fcs_state_address,
			false);

	size_t i = 0;
	// reverse address
	for(i = 0 ; i < frame.address_count * 7; ++i) {
		frame.address[i] = byterev(frame.address[i]);
	}


	unsigned int file_bit_shift = 0;
	while(!feof((FILE *)input)) {
		frame.info_count =
			fread(frame.info,sizeof(*frame.info),info_count,(FILE *)input);

		frame.control = control_field(send_sequence, 0, false );
		send_sequence = (send_sequence + 1) % 8;


		// fcs calculation on non reflected frame
		struct frame_check_state fcs_state = fcs_state_address;

		frame_check(&frame.control, 1,                &frame.fcs, &fcs_state,
				false);
		frame.pid = pid();
		frame_check(&frame.pid,     1,                &frame.fcs, &fcs_state,
				false);
		frame_check(frame.info,     frame.info_count, &frame.fcs, &fcs_state,
				false);

		uint8_t zero[2];
		zero[0] = zero[1] = 0x00;
		frame_check(zero, 2, &frame.fcs, &fcs_state, true);
		frame.fcs = htons(frame.fcs); // Endianness

		// write frame to file
		file_bit_shift = write_frame(file_bit_shift, &frame, output); 
	}
	free(frame.address);
	free(frame.info);
}
