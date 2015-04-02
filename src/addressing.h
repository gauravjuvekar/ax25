#pragma once
#include<stdbool.h>
#include<inttypes.h>

bool is_valid_address(const char *address);
uint8_t *address_from_string(uint8_t *buffer, const char *string);
