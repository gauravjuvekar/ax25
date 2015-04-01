#include<argp.h>
#include<stdio.h>
#include<stdbool.h>
#include "specifications.h"

static char doc[] = "Encodes data into AX.25 frames";
static char args_doc[] = "DESTINATION SOURCE ";

static struct argp_option options[] = {
	{ "repeater", 'r', "CALLSIGN:SSID", 0, "Repeater station CALLSIGN:SSID" },
	{ "input",    'i', "FILE",     0, "Input file" },
	{ "output",   'o', "FILE",     0, "Output file" },
	{ 0 }
};

struct arguments {
	char *routing[ROUTING_MAX_NODES];
	int repeater_count;
	char *input_file;
	char *output_file;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;
	switch (key) {
		case 'i': arguments->input_file  = arg; break;
		case 'o': arguments->output_file = arg; break;
		case 'r': {
			if (arguments->repeater_count + 2 < ROUTING_MAX_NODES) {
				arguments->routing[arguments->repeater_count + 2] = arg;
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
			arguments->routing[state->arg_num] = arg;
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
	doc
};

int main(int argc, char *argv[]) {
	struct arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	return 0;
}
