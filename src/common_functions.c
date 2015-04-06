#include "common_functions.h"

uint8_t byterev(uint8_t byte) {
	return byte_rev_table[byte];
}
