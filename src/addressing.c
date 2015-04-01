#include "addressing.h"
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

bool is_valid_address(const char *address){
	int i = 0;
	while (address[i] && i <= 6) {
		if ( !( isupper(address[i]) || isdigit(address[i]) ||
					address[i] == ':')) {
			return false; // Address must be [A-Z0-9]+ (or delimiter)
		}
		if (address[i] == ':') { // ADDRESS:SSID delimiter
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
