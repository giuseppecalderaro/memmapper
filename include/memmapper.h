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

#ifndef __MEMMAPPER_H__
#define __MEMMAPPER_H__

#define TRUE 		1
#define FALSE		0

#define FIELD(reg, op)  (reg << op##_OFFSET)
#define MASK(op)        ((1 << (op##_OFFSET + op##_WIDTH)) - (1 << op##_OFFSET))
#define VALUE(reg, op)  ((reg & MASK(op)) >> op##_OFFSET)
#define NO_OP		(0x00)

/* Operations.  */
#define CMD_OFFSET      (0)
#define CMD_WIDTH       (4)
#define MEM_OP          (0x01)
#define IO_OP		(0x02)
#define PCI_OP		(0x03)
#ifdef DISASM
#define DISASM_OP       (0x04)
#endif

#define RW_OFFSET       (CMD_OFFSET + CMD_WIDTH)
#define RW_WIDTH        (4)
#define READ_ONLY	(0x00)
#define WRITE_ONLY	(0x01)
#define WRITE_READ	(0x02)

/* Colored output.  */
#define COLOR_OFFSET    (RW_OFFSET + RW_WIDTH)
#define COLOR_WIDTH     (4)
#define COLOR_EN	(0x01)

/* Length.  */
#define LENGTH_OFFSET   (COLOR_OFFSET + COLOR_WIDTH)
#define LENGTH_WIDTH    (4)
#define LEN1            (0x00)
#define LEN2            (0x01)
#define LEN4            (0x02)
#define LEN8            (0x03)

/* Ascii output.  */
#define ASCII_OFFSET    (LENGTH_OFFSET + LENGTH_WIDTH)
#define ASCII_WIDTH     (4)
#define ASCII_EN	(0x01)

/* Mem open flags.  */
#define OPEN_OFFSET     (ASCII_OFFSET + ASCII_WIDTH)
#define OPEN_WIDTH      (4)
#define OPEN_SYNC       (0x01)

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
#define VERSION		"2.00"

#ifdef __MEMMAPPER_C__
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

#endif /* __MEMMAPPER_H__  */
