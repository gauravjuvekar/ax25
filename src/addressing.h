#pragma once
#include<stdbool.h>
#include<inttypes.h>
#include<stddef.h>
#include "frame.h"
#include "ax25encode.h"

bool is_valid_address(const char *address);
uint8_t *address_from_string(uint8_t *buffer, const char *string);
size_t address_field(
		struct frame *dest,
		const char *source[], size_t address_count
);
