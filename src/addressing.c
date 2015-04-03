#include "addressing.h"
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdio.h>
#include<sysexits.h>

bool is_valid_address(const char *address){
	int i = 0;
	while (address[i] && i <= 6) {
		if ( !( isupper(address[i]) || isdigit(address[i]) ||
					address[i] == ':')) {
			return false; // Address must be [A-Z0-9]+ (or delimiter)
		}
		if (address[i] == ':') { // ADDRESS:SSID delimiter
			 /* TODO: use strtoul with errorchecking and remove this mess*/ 
			if (!isdigit(address[i+1])) {
				return false; // SSID must be atleast 1 digit
			}
			if (address[i+2] == 0) {
				return true; // SSID is single digit 0-9, valid address string
			}
			else {
				if (!(isdigit(address[i+2]) && address[i+3] == 0)) {
					return false; // SSID must be atmost 2 digits
				}
				if (atoi(&address[i+1]) <= 15) {
					return true; // SSID must fit in 4 bits i.e. 0-15
				}
				else {
					return false;
				}
			}
		}
		i += 1;
	}
	return false; // Address too long or SSID absent
}


uint8_t *address_from_string(uint8_t *buffer, const char *string) {
	/* Converts a validated address to required binary format 
	 * (ascii in first 7 bits with last bit 0, space padded to 6 bytes + SSID)
	 * stored in uint8_t *buffer which must be 7 bytes long
	 * The address bit and C/H bit of SSID octet need to be set externally
	 *
	 * The function returns the address of the byte AFTER the SSID byte
	 **/
	int i = 0;
	while(string[i] != ':') {
		buffer[i] = ((uint8_t) string[i]) << 1;
		i += 1;
	}
	while(i < 6) {
		buffer[i] = ((uint8_t) ' ') << 1;
		i += 1;
	}

	buffer[i] = ((((uint8_t)strtoul(&string[i],NULL,0)) & 0x0f) << 1) | 0x60;
	/* 011SSID0 , still need to set C/H and extension bit externally*/
	// ^ <--- TODO : in address_field() 
	
	return &buffer[0];
}


size_t address_field(struct frame *dest, const struct arguments *source) {
	/* Fills dest->address with a proper address field of which 
	 * dest->address_count elements are to be used with input from 
	 * argument struct from main
	 * address_count is additionally returned.
	 */
	int nodes = source->repeater_count + 2;
	dest->address = malloc(nodes * 7 * sizeof(*(dest->address)));
	if (dest->address == NULL) {
		fprintf(stderr, "fatal, malloc failed");
		exit(EX_OSERR);
	}
	uint8_t *next = dest->address;

	int i;
	for(i = 0 ; i < nodes ; ++i) {
		next = address_from_string(next, source->routing[i]);
	}
	next -= 1;
	*next |= 0x01;

	dest->address_count = nodes;
	return nodes;
}
