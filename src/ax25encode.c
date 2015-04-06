#include<argp.h>
#include<stdio.h>
#include<stdlib.h>
#include<sysexits.h>
#include<stdbool.h>
#include "ax25encode.h"
#include "specifications.h"
#include "addressing.h"

static char doc[]      = "Encodes data into AX.25 frames";
static char args_doc[] = "DESTINATION SOURCE ";

static struct argp_option options[] = {
{ "repeater", 'r', "CALLSIGN:SSID", 0, "Repeater station CALLSIGN:SSID", 0 },
{ "input",    'i', "FILE",          0, "Input file",                     0 },
{ "output",   'o', "FILE",          0, "Output file",                    0 },
{ 0,          0,   0,               0, 0,                                0 }
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;
	switch (key) {
		case 'i': arguments->input_file  = arg; break;
		case 'o': arguments->output_file = arg; break;
		case 'r': {
			if (arguments->repeater_count + 2 < ROUTING_MAX_NODES) {
				if (is_valid_address(arg)) {
					arguments->routing[arguments->repeater_count + 2] = arg;
				}
				else {
					argp_failure(state, 1, 0, 
							"invalid address format for repeater %s", arg);
				}
				arguments->repeater_count += 1;
			}
			else {
				argp_failure(state, 1, 0,
						"too many repeaters, max %d allowed", REPEATERS_MAX);
			}
			break;
		}
		case ARGP_KEY_INIT: {
			arguments->repeater_count = 0;
			arguments->input_file     = "";
			arguments->output_file    = "";
		}
		case ARGP_KEY_ARG: {
			if (state->arg_num >= 2) {
				argp_usage(state);
			}
			if (arg != NULL) {
				if (is_valid_address(arg)) {
					arguments->routing[state->arg_num] = arg;
				}
				else {
					argp_failure(state, 1, 0, 
							"invalid address format for argument %s", arg);
				}
			}
			break;
		}
		case ARGP_KEY_END: {
			if (state->arg_num < 2) {
				argp_usage(state);
			}
			break;
		}
		default: return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {
	options,
	parse_opt,
	args_doc,
	doc,
	0, 0, 0
};

int main(int argc, char *argv[]) {
	struct arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	FILE *input = fopen(arguments.input_file, "rb");
	if (input == NULL) {
		fprintf(stderr, "Cannot open input file");
		exit(EX_NOINPUT);
	}
	FILE *output = fopen(arguments.output_file, "wb");
	if (output == NULL) {
		fprintf(stderr, "Cannot open output file");
		exit(EX_CANTCREAT);
	}





	return 0;
}
