#pragma once
#include<inttypes.h>

struct frame{
	uint8_t *address; // malloced pointer to a complete address field
	unsigned int address_count; // with address_count routing addresses 
	                            // == address_count * 7 bytes
};
