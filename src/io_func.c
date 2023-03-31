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

#define __IO_FUNC_C__
#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <debug_func.h>
#include <memmapper.h>
#include <lib_func.h>

void io_rw(unsigned short int port, unsigned long data, int write, int color_en, int length)
{
	const char *color = NULL;
	unsigned long read_data = 0;

	/* Set userspace permission on I/O ports.  */
	iopl(3);

	/* I/O ports are pow(2, 16).  */
	if(data > 0xFFFFFFFF) {
		PRINTD("I/O ports can be at most 32 bits wide, truncating...\n");
		data &= 0xFFFFFFFFUL;
	}

	if(color_en)
		printf("IO port: " "%s" "0x%.4x\n" "%s", colors[yellow], port, colors[default_color]);
	else
		printf("IO port: 0x%.4x\n", port);

	if(write) {
		PRINTD("I/O port write operation.\n");
		switch(length) {
		case 1:
			if(color_en)
				printf("Writing data: " "%s" "0x%.2lx" "%s" "\n", colors[yellow], data, colors[default_color]);
			else
				printf("Writing data: 0x%.2lx\n", data);
			outb((unsigned char)data, port);
			break;
		case 2:
			if(color_en)
				printf("Writing data: " "%s" "0x%.4lx" "%s" "\n", colors[yellow], data, colors[default_color]);
			else
				printf("Writing data: 0x%.4lx\n", data);
			outw((unsigned short int)data, port);
			break;
		case 4:
		default:
			if(color_en)
				printf("Writing data: " "%s" "0x%.8lx" "%s" "\n", colors[yellow], data, colors[default_color]);
			else
				printf("Writing data: 0x%.8lx\n", data);
			outl(data, port);
			break;
		}
	}

	if ((write == READ_ONLY) || (write == WRITE_READ)) {
		PRINTD("I/O port read operation.\n");
		color = colors[light_cyan];
		switch(length) {
		case 1:
			read_data = inb(port);
			if(color_en) {
				if(write == WRITE_READ) {
					if(read_data != data) {
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
			if(color_en) {
				if(write == WRITE_READ) {
					if(read_data != data) {
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
			if(color_en) {
				if(write == WRITE_READ) {
					if(read_data != data) {
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

static int io_op(unsigned long op, const char *resource, const char *device, const char *value)
{
	unsigned short int port;
	unsigned long data;
	int color = VALUE(op, COLOR);
	int length = (1 << VALUE(op, LENGTH));
	int write = VALUE(op, RW);

	PRINTD("%s: op %.8x - resource %s - device %s - value %s\n",
	       __FUNCTION__,
	       op,
	       resource,
	       device ? : "No Dev",
	       value? : "No value");

	/* Decode resources.  */
	port = hex_encoder(resource, 0);
	if(value)
		data = hex_encoder(value, 0);

	PRINTD("%s: port %#x - data %#x - length %#x - write %#x - color %#x\n",
	       __FUNCTION__,
	       port,
	       data,
	       length,
	       write,
	       color);

	/* Perform requested operation.  */
	io_rw(port, data, write, color, length);

	return EXIT_SUCCESS;
}

int register_io(void **action)
{
	*action = (void *)io_op;

	return EXIT_SUCCESS;
}
