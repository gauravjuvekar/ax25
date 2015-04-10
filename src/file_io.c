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



uint8_t write_frame(
		uint8_t bit_rotate_mask,
		struct frame* frame,
		FILE *output) {
	/* Start with flag filled from bit_rotate_mask
	 * Should be initially set to 0x80u
	 */

	// Byterev stuff
	// Address is already bytereved
	frame->control = byterev(frame->control);
	frame->pid     = byterev(frame->pid);
	size_t i;
	for(i = 0 ; i < frame->info_count; ++i) {
		frame->info[i] = byterev(frame->info[i]);
	}

	// bit_stuff and write
	uint8_t buffer[1024];
	buffer[0] = 0;
	struct bit_stuff_state stuff_state;
	
	stuff_state.get_state.src_mask     = UINT8_T_MSB_MASK;
	stuff_state.set_state.dest_mask    = bit_rotate_mask;
	stuff_state.get_state.src_index    = 0;
	stuff_state.set_state.dest_index   = 0;
	stuff_state.set_state.dest_filled  = false;
	stuff_state.get_state.src_consumed = false;
	stuff_state.contiguous_bit_count   = 0;

	const uint8_t flag = 0x7eu;
	// write flag
	while(!stuff_state.get_state.src_consumed) {
		set_bit(buffer, 1024,
				get_bit(&flag, 1, &stuff_state.get_state),
				&stuff_state.set_state);
	}

	stuff_state.get_state.src_consumed = false;
	stuff_state.get_state.src_index    = 0;
	stuff_state.get_state.src_mask  = UINT8_T_MSB_MASK;
	bit_stuff(
			buffer, 1024,
			frame->address, 7 * frame->address_count,
			&stuff_state);

	stuff_state.get_state.src_consumed = false;
	stuff_state.get_state.src_index    = 0;
	stuff_state.get_state.src_mask  = UINT8_T_MSB_MASK;
	bit_stuff(
			buffer, 1024,
			&frame->control, 1,
			&stuff_state);

	stuff_state.get_state.src_consumed = false;
	stuff_state.get_state.src_index    = 0;
	stuff_state.get_state.src_mask  = UINT8_T_MSB_MASK;
	bit_stuff(
			buffer, 1024,
			&frame->pid, 1,
			&stuff_state);

	stuff_state.get_state.src_consumed = false;
	stuff_state.get_state.src_index    = 0;
	stuff_state.get_state.src_mask  = UINT8_T_MSB_MASK;
	bit_stuff(
			buffer, 1024,
			frame->info, frame->info_count,
			&stuff_state);

	stuff_state.get_state.src_consumed = false;
	stuff_state.get_state.src_index    = 0;
	stuff_state.get_state.src_mask  = UINT8_T_MSB_MASK;
	bit_stuff(
			buffer, 1024,
			(uint8_t *)&frame->fcs, 2,
			&stuff_state);

	stuff_state.get_state.src_consumed = false;
	stuff_state.get_state.src_index    = 0;
	stuff_state.get_state.src_mask  = UINT8_T_MSB_MASK;
	// write flag
	while(!stuff_state.get_state.src_consumed) {
		set_bit(buffer, 1024,
				get_bit(&flag, 1, &stuff_state.get_state),
				&stuff_state.set_state);
	}
	


	// get previous byte
	fseek(output, -1, SEEK_CUR);
	uint8_t byte_buffer = 0;
	fread(&byte_buffer, 1, 1, output);
	fseek(output, -1, SEEK_CUR);

	// OR it with flag beginning
	uint8_t tmp_mask = bit_rotate_mask;
	while(tmp_mask) {
		byte_buffer &= ~tmp_mask;
		tmp_mask >>= 1;
	}
	buffer[0] |= byte_buffer ;
	buffer[stuff_state.set_state.dest_index + 1] = 0;
	fwrite(buffer, 1, stuff_state.set_state.dest_index + 1 , output);

	return stuff_state.set_state.dest_mask ;
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


	size_t send_sequence = 0;
	uint8_t file_bit_shift_mask = UINT8_T_MSB_MASK;
	while(!feof((FILE *)input)) {
		frame.info_count =
			fread(frame.info,sizeof(*frame.info),info_count,(FILE *)input);

		frame.control = control_field(send_sequence, 0, false );
		send_sequence = (send_sequence + 1) % 8;


		// fcs calculation on non reflected frame
		struct frame_check_state fcs_state = fcs_state_address;

		frame_check(&frame.control, 1, &frame.fcs, &fcs_state, false);
		frame.pid = pid();
		frame_check(&frame.pid,     1, &frame.fcs, &fcs_state, false);
		frame_check(frame.info, frame.info_count, &frame.fcs, &fcs_state,
				false);

		uint8_t zero[2];
		zero[0] = zero[1] = 0x00u;
		frame_check(zero, 2, &frame.fcs, &fcs_state, true);
		frame.fcs = htons(frame.fcs); // Endianness

		// write frame to file
		file_bit_shift_mask = write_frame(file_bit_shift_mask, &frame, output); 
	}
	free(frame.address);
	free(frame.info);
}
