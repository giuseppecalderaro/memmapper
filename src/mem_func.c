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

#define __MEM_FUNC_C__
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <sys/user.h>

#include <debug_func.h>
#include <lib_func.h>
#include <mem_func.h>
#include <memmapper.h>

static int memory_dumper(unsigned long start, unsigned long end, unsigned long data, int increment, int write, int color_en, int length, int ascii_en, int fd)
{
	unsigned char progress_bar[] = "-\\|/";
	char buffer[16];
	const char *color = NULL;
	unsigned long read_data;
	int error = 0;
	int i, j;

	PRINTD("begin address: %#lx - end address: %#lx - length: %#x - operation: %#x\n",
	       start,
	       end,
	       length,
	       write);

	for(i = 0, j = 0, read_data = data; i < end - start; i += length, j++, data += increment, read_data += increment) {
		if(write) {
			if(write == WRITE_ONLY)
				printf("%c", progress_bar[(i >> 2) & 0x3]);
			error = memory_rw(start + i, &data, length, increment, write, fd);
			if(error)
				return EXIT_FAILURE;
			if(write == WRITE_ONLY)
				printf(DELETE);
		}

		if((write ==  READ_ONLY) || (write == WRITE_READ)) {
			error = memory_rw(start + i, &data, length, increment, READ_ONLY, fd);
			if(error) {
				printf("Error reading!\n");
				return EXIT_FAILURE;
			}

			if(!(i % 16)) {
				j = 0;
				if(color_en)
					printf("\n" "%s" "%." NIBBLES_PER_WORD "lx - ", colors[yellow], start + i);
				else
					printf("\n%." NIBBLES_PER_WORD "lx - ", start + i);
			}
			if(!(i % 4))
				printf("\t");

			/* Save read data.  */
			buffer[j] = (char)data;

			switch(length) {
			case 2:
				if(color_en) {
					color = colors[light_cyan];
					if(write == WRITE_READ) {
						if(read_data != data) {
							color = colors[red];
						} else {
							color = colors[light_green];
						}
					}
					printf("%s" "%.4lx ", color, data);
				} else {
					printf("%.4lx ", data);
				}
				break;
			case 4:
				if(color_en) {
					color = colors[light_cyan];
					if(write == WRITE_READ) {
						if(read_data != data) {
							color = colors[red];
						} else {
							color = colors[light_green];
						}
					}
					printf("%s" "%.8lx ", color, data);
				} else {
					printf("%.8lx ", data);
				}
				break;
			case 8:
				if(color_en) {
					color = colors[light_cyan];
					if(write == WRITE_READ) {
						if(read_data != data) {
							color = colors[red];
						} else {
							color = colors[light_green];
						}
					}
					printf("%s" "%." NIBBLES_PER_WORD "lx ", color, data);
				} else {
					printf("%." NIBBLES_PER_WORD "lx ", data);
				}
				break;
			case 1:
			default:
				if(color_en) {
					color = colors[light_cyan];
					if(write == WRITE_READ) {
						if(read_data != data) {
							color = colors[red];
						} else {
							color = colors[light_green];
						}
					}
					printf("%s" "%.2lx ", color, data);
				} else {
					printf("%.2lx ", data);
				}
				break;
			}

			if(!((i + 1) % 16)) {
				if(ascii_en) {
					printf("\t");
					for(j = 0; j < 16; j++) {
						if(isprint(buffer[j]))
							printf("%c", (char)buffer[j]);
						else
							printf(".");
					}
				}
			}
		}
	}

	if(color_en)
		printf("%s" "\n", colors[default_color]);
	else
		printf("\n");

	return EXIT_SUCCESS;
}

static int mem_op(unsigned long op, const char *resource, const char *device, const char *value)
{
	unsigned long start;
	unsigned long end = 0;
	unsigned long data = 0;
	unsigned long increment = 0;
	char *temp;
	/* Op fields.  */
	int write = VALUE(op, RW);
	int color = VALUE(op, COLOR);
	int length = (1 << VALUE(op, LENGTH));
	int ascii = VALUE(op, ASCII);
	/* File descriptor.  */
	int fd;

	PRINTD("%s: write %#x - color %#x - length %#x - ascii %#x\n",
	       __FUNCTION__,
	       write,
	       color,
	       length,
	       ascii);

	/* Open device.  */
	fd = open(device ? device : DEVICE, (op & OPEN_SYNC) ? O_RDWR | O_SYNC : O_RDWR);
	if(fd == -1) {
		printf("Cannot open file descriptor...\n"
		       "quitting...\n");
		return EXIT_FAILURE;
	}

	/* Decoding of resource string.  */
	if(temp = strchr(resource, ':')) {
		*temp++ = '\0'; /* Set to zero the separator.  */
		start = hex_encoder(resource, 0);
		end = hex_encoder(temp, 0);
	} else {
		if(temp = strchr(resource, '+')) {
			*temp++ = '\0'; /* Set to zero the separator.  */
			start = hex_encoder(resource, 0);
			end = start + hex_encoder(temp, 0);
		} else {
			start = hex_encoder(resource, 0);
			end = start + length;
		}
	}

	if(value) {
		/* Decoding of value string.  */
		if(temp = strchr(value, '+')) {
			*temp++ = '\0'; /* Set to zero the separator.  */
			data = hex_encoder(value, 0);
			increment = hex_encoder(temp, 0);
		} else {
			data = hex_encoder(value, 0);
		}
	}

	PRINTD("%s: start %.8x - end %.8x - data %.8x - increment %.8x\n",
	       __FUNCTION__,
	       start,
	       end,
	       data,
	       increment);

	memory_dumper(start, end, data, increment, write, color, length, ascii,  fd);

	/* Close device.  */
	close(fd);

	return EXIT_SUCCESS;
}

int register_mem(void **action)
{
	*action = (void *)mem_op;

	return EXIT_SUCCESS;
}
