#include "file_io.h"
#include<stdlib.h>
#include<stdio.h>
#include "frame.h"
#include<sysexits.h>
#include "addressing.h"
#include "pid.h"

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
	frame.pid = pid();










	free(frame.address);
	free(frame.info);
}
