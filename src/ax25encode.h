#pragma once
#include "specifications.h"

struct arguments {
	char *routing[ROUTING_MAX_NODES];
	int repeater_count;
	char *input_file;
	char *output_file;
};
