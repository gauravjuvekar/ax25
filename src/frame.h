#pragma once
#include<inttypes.h>
#include<stddef.h>

struct frame {
	uint8_t  *address;      // malloced pointer to a complete address field
	size_t   address_count; // with address_count routing addresses
	                        // == address_count * 7 bytes

	uint8_t  control;       // control field
	uint8_t  pid;           // pid field

	uint8_t  *info;         // malloced pointer to info field
	size_t   info_count;    // with info_count * sizeof(*info) bytes

	uint16_t fcs;           // frame check sequence
};
