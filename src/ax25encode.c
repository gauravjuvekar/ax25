#include<argp.h>
#include<stdio.h>
#include<stdbool.h>

static char doc[] = "Encodes data into AX.25 frames";
static char args_doc[] = "";

static struct argp_option options[] = {
	{ "source",      's', "SOURCE",      0, "Source station CALLSIGN:SSID" },
	{ "destination", 'd', "DESTINATION", 0,
		"Destination station CALLSIGN:SSID" },
	{ "repeater",    'r', "REPEATER",    OPTION_ARG_OPTIONAL,
		"Repeater station CALLSIGN:SSID" },
	{ "input",       'i', "INPUT",       0, "Input file" },
	{ "output",      'o', "OUTPUT",      0, "Output file" },
	{ 0 }
};

struct arguments {
	char *SOURCE;
	char *DESTINATION;
	/*char *REPEATERS[];*/
	char *INPUT;
	char *OUTPUT;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;
	switch (key) {
		case 's': arguments->SOURCE      = arg; break;
		case 'd': arguments->DESTINATION = arg; break;
		case 'i': arguments->INPUT       = arg; break;
		case 'o': arguments->OUTPUT      = arg; break;
		case ARGP_KEY_ARG: return 0;
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

	printf("%s\n", arguments.SOURCE);
	printf("%s\n", arguments.DESTINATION);
	printf("%s\n", arguments.INPUT);
	printf("%s\n", arguments.OUTPUT);

	return 0;
}
