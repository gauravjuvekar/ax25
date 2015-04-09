#pragma once
#include<inttypes.h>
#include<stdbool.h>

uint8_t control_field(uint8_t send_seq, uint8_t recieve_seq, bool poll);

