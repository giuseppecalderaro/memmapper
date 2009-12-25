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

#include <sys/io.h>
#include <stdio.h>

#include <debug_func.h>
#include <memmapper.h>

void io_rw(unsigned short int port, unsigned long *data, int length, int op)
{
	const char *color = NULL;
	unsigned long read_data = 0;
	
	/* Set userspace permission on I/O ports.  */
	iopl(3);
	
	/* I/O ports are pow(2, 16).  */
	if(*data > 0xFFFFFFFF) {
		PRINTD("I/O ports can be at most 32 bits wide, truncating...\n");
		*data &= 0xFFFFFFFFUL;
	}
	
	if(op & COLORS)
		printf("IO port: " "%s" "0x%.4x\n" "%s", colors[yellow], port, colors[default_color]);
	else
		printf("IO port: 0x%.4x\n", port);		
		
	if(op & WRITE_ONLY) {
		PRINTD("I/O port write operation.\n");
		switch(length) {
		case 1:
			if(op & COLORS)
				printf("Writing data: " "%s" "0x%.2lx" "%s" "\n", colors[yellow], *data, colors[default_color]);
			else
				printf("Writing data: 0x%.2lx\n", *data);
			outb((unsigned char)*data, port);			
			break;
		case 2:
			if(op & COLORS)
				printf("Writing data: " "%s" "0x%.4lx" "%s" "\n", colors[yellow], *data, colors[default_color]);
			else
				printf("Writing data: 0x%.4lx\n", *data);
			outw((unsigned short int)*data, port);			
			break;
		case 4:
		default:
			if(op & COLORS)
				printf("Writing data: " "%s" "0x%.8lx" "%s" "\n", colors[yellow], *data, colors[default_color]);
			else
				printf("Writing data: 0x%.8lx\n", *data);
			outl(*data, port);			
			break;	
		}	
	} 

	if(((op & (~COLORS)) == READ_ONLY) || ((op & (~COLORS)) & WRITE_READ)) {
		PRINTD("I/O port read operation.\n");
		color = colors[light_cyan];
		switch(length) {
		case 1:
			read_data = inb(port);
			if(op & COLORS) {
				if(op & WRITE_READ) {
					if(read_data != *data) {
						color = colors[red];				
					} else {
						color = colors[light_green];
					}
				}
				printf("Read data: " "%s" "0x%.2lx" "%s" "\n", color, read_data, colors[default_color]);				
			} else {
				printf("Read data: 0x%.2lx\n", read_data);								
			}
			break;
		case 2:
			read_data = inw(port);
			if(op & COLORS) {
				if(op & WRITE_READ) {
					if(read_data != *data) {
						color = colors[red];				
					} else {
						color = colors[light_green];
					}
				}
				printf("Read data: " "%s" "0x%.4lx" "%s" "\n", color, read_data, colors[default_color]);				
			} else {
				printf("Read data: 0x%.4lx\n", read_data);				
			}
			break;
		case 4:
		default:
			read_data = inl(port);
			if(op & COLORS) {
				if(op & WRITE_READ) {
					if(read_data != *data) {
						color = colors[red];				
					} else {
						color = colors[light_green];
					}
				}
				printf("Read data: " "%s" "0x%.8lx" "%s" "\n", color, read_data, colors[default_color]);				
			} else {
				printf("Read data: 0x%.8lx\n", read_data);								
			}
			break;	
		}			
	}
}
