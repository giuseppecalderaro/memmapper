/********************************************************************************
 * This file is part of memmapper.												*
 * 										                                        *
 * memmapper is free software: you can redistribute it and/or modify		    *
 * it under the terms of the GNU General Public License as published by		    *
 * the Free Software Foundation, either version 3 of the License, or		    *
 * (at your option) any later version.						                    *
 * memmapper is distributed in the hope that it will be useful,			        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of		        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		        *
 * GNU General Public License for more details.					                *
 * You should have received a copy of the GNU General Public License		    *
 * along with memmapper.  If not, see <http://www.gnu.org/licenses/>.		    *
 * 										                                        *
 * Author: 	Giuseppe Calderaro						                            *
 * Email: 	giuseppecalderaro@gmail.com					                        *
 * 										                                        *
 ********************************************************************************/

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <lib_func.h>
#include <debug_func.h>

unsigned long hex_encoder(const char *address, int *sign)
{
	int base;
	int length;
	int unit = 1;
	char *unit_ptr = NULL;
	char *number;
	unsigned long result = 0;
	int i;

	if(address == NULL) {
		PRINTD("Error: null pointer\n");
		return 0;
	}

	/* Initialize sign.  */
	if(sign)
		*sign = 1;

	/* Size is in kilobyte.  */
	if((unit_ptr = strchr(address, 'k')) || (unit_ptr = strchr(address, 'K'))) {
		*unit_ptr = 0;
		unit = KB;
	}
	/* Size is in megabyte.  */
	if((unit_ptr = strchr(address, 'm')) || (unit_ptr = strchr(address, 'M'))) {
		*unit_ptr = 0;
		unit = MB;
	}
	/* Size is in gigabyte.  */
	if((unit_ptr = strchr(address, 'g')) || (unit_ptr = strchr(address, 'G'))) {
		*unit_ptr = 0;
		unit = GB;
	}

	switch(address[0]) {
	case '0':
		if((address[1] == 'x') || (address[1] == 'X')) {
			base = 16;
			number = (char *)address + 2;
			length = strlen(address) - 2;
		} else {
			base = 8;
			number = (char *)address + 1;
			length = strlen(address) - 1;
		}
		break;
	default:
		base = 10;
		if(address[0] == '-') {
			if (sign)
				*sign = -1;
			address++;
		}
		number = (char *)address;
		length = strlen(address);
		break;
	}

	for(i = 0; i < length; i++) {
		switch(number[i]) {
		case '0' ... '9':
			number[i] -= '0';
			break;
		case 'A' ... 'F':
			number[i] = tolower(number[i]);
		case 'a' ... 'f':
			number[i] -= 'a' - 10;
			break;
		default:
			/* What else is there?  */
			break;
		}
	}

	/* Evaluate final result.  */
	for(i = 0; i < length; i++)
		result += (unsigned long)number[i] * (unsigned long)pow(base, length - i - 1);
	/* Apply multiplier.  */
	result *= unit;

	return result;
}
