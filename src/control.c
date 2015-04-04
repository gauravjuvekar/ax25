#include "control.h"

uint16_t control_field(uint8_t send_seq, uint8_t recieve_seq, bool poll) {
	/* Returns a 16 bit control field value
	 * rrrrrrrpsssssss0
	 * r = receive, p = poll, s = send 
	 * send_seq and recieve_seq must be between 0-128, else their msb will 
	 * be silently dropped.
	 */
	uint16_t control = 0;
	control |= ((uint16_t)recieve_seq) << 9;
	control |= (uint16_t)(send_seq << 1);
	if(poll) {
		control |= 1 << 8;
	}
	return control;
}
