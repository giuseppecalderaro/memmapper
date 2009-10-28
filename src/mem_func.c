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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#ifdef DISASM
#include <udis86.h>
#endif
#include <unistd.h>
#include <sys/user.h>

#include "inc/debug_func.h"
#include "inc/lib_func.h"
#include "inc/mem_func.h"
#include "inc/memmapper.h"

int memory_rw(unsigned long physical, unsigned long *data, int length, int increment, int op, int fd)
{
	void *address;
	unsigned long read_data = 0;
	int pagesize;
	
	pagesize = sysconf(_SC_PAGESIZE);
	if(pagesize == -1) {
		perror("sysconf");
		return EXIT_FAILURE;
	}

	address = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, physical & PAGE_MASK);
	if(address == MAP_FAILED) {
		printf("Cannot mmap file descriptor.\n"
				"quitting...");
		return EXIT_FAILURE;
	}
	
	if(op & WRITE_ONLY) {
		switch(length) {
		case 1: 
			*data &= 0xFF;
			writeb(address + (physical & (~PAGE_MASK)), *data);
			if(op & WRITE_READ) {
				read_data = readb(address + (physical & (~PAGE_MASK)));
			} else {
				read_data = *data;
			}
			break;
		case 2:	
			*data &= 0xFFFF;
			writew(address + (physical & (~PAGE_MASK)), *data);
			if(op & WRITE_READ) {
				read_data = readw(address + (physical & (~PAGE_MASK)));
			} else {
				read_data = *data;
			}
			break;
		case 4:
#ifdef ARCH64
			*data &= 0xFFFFFFFF;
			writedw(address + (physical & (~PAGE_MASK)), *data);
			if(op & WRITE_READ) {
				read_data = readdw(address + (physical & (~PAGE_MASK)));
			} else {
				read_data = *data;
			}
			break;
		case 8:
#endif
		default:
			writel(address + (physical & (~PAGE_MASK)),*data);
			if(op & WRITE_READ) {
				read_data = readl(address + (physical & (~PAGE_MASK)));
			} else {
				read_data = *data;
			}
			break;
		}
	} else {
		switch(length) {
		case 1:
			*data = readb(address + (physical & (~PAGE_MASK)));
			break;
		case 2:
			*data = readw(address + (physical & (~PAGE_MASK)));				
			break;
		case 4:
#ifdef ARCH64
			*data = readdw(address + (physical & (~PAGE_MASK)));			
			break;
		case 8:
#endif
		default:
			*data = readl(address + (physical & (~PAGE_MASK)));
			break;
		}	
	}
	munmap(address, pagesize);
	
	return EXIT_SUCCESS;
}

int memory_dumper(unsigned long from, char sep, unsigned long to, unsigned long *data, int length, int increment, int op, int fd)
{
	unsigned char progress_bar[] = "-\\|/";
	char buffer[16];
	const char *color = NULL;
	unsigned long read_data;
	int error = 0;
	int i, j;
	unsigned int size;
	
	if((length > 4) || (length == -1))
#ifdef ARCH64
		length = 8;
#else
		length = 4;
#endif
	
	PRINTD("begin address: %#lx\n"
			"end address: %#lx\n"
			"length: %#x\n"
			"operation: %#x\n",
			from, to, length, op);
	
	/* Evalute action to perform.  */
	if(to) {
		switch(sep) {
		case ':':
			if(from > to) {
				printf("Start address should be LOWER than end address. Quitting...\n");
				return -1;		
			}
			size = to - from;
			break;
		case '+':
			size = to;
			break;
		}
	} else {
		size = length;
	}
		
	for(i = 0, j = 0, read_data = *data; i < size; i += length, j++, *data += increment, read_data += increment) {
		
		if(op & WRITE_ONLY) {
			if(!(op & WRITE_READ))
				printf("%c", progress_bar[(i >> 2) & 0x3]);			
			error = memory_rw(from + i, data, length, increment, op, fd);
			if(error)
				return -1;
			if(!(op & WRITE_READ))			
				printf(DELETE);
		} 
		
		if(((op & (~(COLORS | ASCII))) == READ_ONLY) || ((op & (~(COLORS | ASCII))) & WRITE_READ)) {
			error = memory_rw(from + i, data, length, increment, READ_ONLY, fd);
			if(error) {
				printf("Error reading!\n");
				return -1;	
			}
			
			if(!(i % 16)) {	
				j = 0;
				if(op & COLORS)
					printf("\n" "%s" "%." NIBBLES_PER_WORD "lx - ", colors[yellow], from + i);
				else
					printf("\n%." NIBBLES_PER_WORD "lx - ", from + i);				
			}
			if(!(i % 4))
				printf("\t");
			
			/* Save read data.  */
			buffer[j] = (char)*data;
			
			switch(length) {
			case 2:	
				if(op & COLORS) {
					color = colors[light_cyan];
					if(op & WRITE_READ) {
						if(read_data != *data) {
							color = colors[red];
						} else {
							color = colors[light_green];
						}
					}
					printf("%s" "%.4lx ", color, *data);					
				} else {
					printf("%.4lx ", *data);
				}
				break;
			case 4:
				if(op & COLORS) {
					color = colors[light_cyan];
					if(op & WRITE_READ) {
						if(read_data != *data) {
							color = colors[red];
						} else {
							color = colors[light_green];
						}
					}
					printf("%s" "%.8lx ", color, *data);					
				} else {
					printf("%.8lx ", *data);
				}
				break;
#ifdef ARCH64
			case 8:
				if(op & COLORS) {
					color = colors[light_cyan];
					if(op & WRITE_READ) {
						if(read_data != *data) {
							color = colors[red];
						} else {
							color = colors[light_green];
						}
					}
					printf("%s" "%." NIBBLES_PER_WORD "lx ", color, *data);					
				} else {
					printf("%." NIBBLES_PER_WORD "lx ", *data);	
				}
				break;
#endif
			case 1:
			default:
				if(op & COLORS) {
					color = colors[light_cyan];
					if(op & WRITE_READ) {
						if(read_data != *data) {
							color = colors[red];
						} else {
							color = colors[light_green];
						}
					}
					printf("%s" "%.2lx ", color, *data);
				} else {
					printf("%.2lx ", *data);						
				}
				break;
			}
			
			if(!((i + 1) % 16)) {
				if(op & ASCII) {
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
	
	if(op & COLORS)
		printf("%s" "\n", colors[default_color]);
	else
		printf("\n");
	
	return 0;
}

#ifdef DISASM
int memory_disassemble(unsigned long from, unsigned long to, int op, int fd)
{
	ud_t ud_obj;
	char *buffer = NULL;
	int error = 0;
	int i;
	
	PRINTD("begin address: %#lx\n"
			"end address: %#lx\n"
			"length: %#x\n"
			"operation: %#x\n",
			from, to, 1, op);
	
	/* Evalute action to perform.  */
	if(to) {
		if(from > to) {
			printf("Start address should be LOWER than end address. Quitting...\n");
			return -1;		
		}		
	} else {
		printf("Invalid range...\n");
		goto end;
	}

	buffer = (char *)malloc(to - from + 1);
	if(buffer == NULL) {
		printf("Can't allocate buffer memory...\n");
		goto end;
	}
	memset(buffer, 0, to - from + 1);

	for(i = 0; i < (to - from); i += 1) {		
		error = memory_rw(from + i, (unsigned long *)(buffer + i), 1, 0, READ_ONLY, fd);
		if(error) {
			printf("Error reading!\n");
			return -1;	
		}
	}

	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, buffer, to - from + 1);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_ATT);
	ud_set_pc(&ud_obj, from);

	while (ud_disassemble(&ud_obj)) {
		if(op & COLORS) {		       
			printf("%s" "%lx:\t", colors[yellow], ud_insn_off(&ud_obj));
			printf("%s" "%20s\t - \t", colors[light_purple], ud_insn_hex(&ud_obj));
			printf("%s" "%s\n", colors[light_cyan], ud_insn_asm(&ud_obj));
		} else {
			printf("%lx:\t", ud_insn_off(&ud_obj));
			printf("%20s\t - \t", ud_insn_hex(&ud_obj));
			printf("%s\n", ud_insn_asm(&ud_obj));
		}
	}
	
	free(buffer);
	
end:
	return 0;
}
#endif /* DISASM  */
