/********************************************************************************
 * This file is part of memmapper.						*
 * 										*
 * memmapper is free software: you can redistribute it and/or modify		*
 * it under the terms of the GNU General Public License as published by		*
 * the Free Software Foundation, either version 3 of the License, or		*
 * (at your option) any later version.						*
 * memmapper is distributed in the hope that it will be useful,			*
 * but WITHOUT ANY WARRANTY; without even the implied warranty of		*
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		*
 * GNU General Public License for more details.					*
 * You should have received a copy of the GNU General Public License		*
 * along with memmapper.  If not, see <http://www.gnu.org/licenses/>.		*
 * 										*
 * Author: 	Giuseppe Calderaro						*
 * Email: 	giuseppecalderaro@gmail.com					*
 * 										*
 ********************************************************************************/

#ifndef MEMMAPPER_H_
#define MEMMAPPER_H_

#define DEVICE		"/dev/mem"

#define TRUE 		1
#define FALSE		0

/* Actions.  */
#define MEM_OP		0x01
#define IO_OP		0x02
#define PCI_OP		0x03
#define DISASSEMBLE     0x04

/* Ops.  */
#define READ_ONLY	0x00
#define WRITE_ONLY	0x01
#define WRITE_READ	0x02

/* Colored output.  */
#define COLORS		0x80
#define NO_COLORS	0x00

#define ASCII		0x04

/* Colors definitions.  */
enum {
	default_color,
	black,
	blue,
	green,
	cyan,
	red,
	purple,
	brown,
	light_gray,
	dark_gray,
	light_blue,
	light_green,
	light_cyan,
	light_red,
	light_purple,
	yellow,
	white
};

#define AUTHOR		"Giuseppe Calderaro - <giuseppecalderaro@gmail.com>"
#define LICENSE		"GPL"
#define VERSION		"1.43"

#ifdef MEMMAPPER_C_
const char *colors[] = {
		"\033[0m",
		"\033[0;30m",
		"\033[0;34m",
		"\033[0;32m",
		"\033[0;36m",
		"\033[0;31m",
		"\033[0;35m",
		"\033[0;33m",
		"\033[0;37m",
		"\033[1;30m",
		"\033[1;34m",
		"\033[1;32m",
		"\033[1;36m",
		"\033[1;31m",
		"\033[1;35m",
		"\033[1;33m",
		"\033[1;37m"
};
#else
extern const char *colors[];
#endif

void cursor(int on);
void usage(void);

#endif /*MEMMAPPER_H_*/
