#include "control.h"

uint8_t control_field(uint8_t send_seq, uint8_t recieve_seq, bool poll) {
	/* Returns a 8 bit control field value
	 * rrrpsss0
	 * r = receive, p = poll, s = send 
	 * send_seq and recieve_seq must be between 0-7, else their higher bits 
	 * will 
	 * be silently dropped.
	 */
	uint8_t control = 0;
	control |= ((uint8_t)recieve_seq & 0x07) << 5;
	control |= ((uint8_t)send_seq    & 0x07) << 1;
	if(poll) {
		control |= 0x1 << 4;
	}
	return control;
}
