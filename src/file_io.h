#pragma once
#include<stdio.h>

void encode_main_loop(
		const FILE *input, FILE *output,
		size_t info_count,
		const char *addresses[], size_t address_count
);


