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

#define __DISASM_FUNC_C__
#ifdef DISASM
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <udis86.h>
#include <unistd.h>
#include <sys/user.h>
#include <debug_func.h>
#include <lib_func.h>
#include <mem_func.h>
#include <disasm_func.h>
#include <memmapper.h>

static int memory_disasm(unsigned long start, unsigned long end, int color_en, int fd)
{
	ud_t ud_obj;
	char *buffer = NULL;
	int error = 0;
	int i;

	PRINTD("begin address: %#lx - end address: %#lx - length: %#x\n",
	       start,
	       end,
	       1);

	/* Evalute action to perform.  */
	if(end) {
		if(start > end) {
			printf("Start address should be LOWER than end address. Quitting...\n");
			return EXIT_FAILURE;
		}
	} else {
		printf("Invalid range...\n");
		goto end;
	}

	buffer = (char *)malloc(end - start + 1);
	if(buffer == NULL) {
		printf("Can't allocate buffer memory...\n");
		goto end;
	}
	memset(buffer, 0, end - start + 1);

	for(i = 0; i < (end - start); i += 1) {
		error = memory_rw(start + i, (unsigned long *)(buffer + i), 1, 0, READ_ONLY, fd);
		if(error) {
			printf("Error reading!\n");
			return EXIT_FAILURE;
		}
	}

	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, buffer, end - start + 1);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_ATT);
	ud_set_pc(&ud_obj, start);

	while (ud_disassemble(&ud_obj)) {
		if(color_en) {
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
	return EXIT_SUCCESS;
}


static int disasm_op(unsigned long op, const char *resource, const char *device, const char *value)
{
	unsigned long start;
	unsigned long end = 0;
	char *temp;
	/* Op fields.  */
	int color = VALUE(op, COLOR);
	/* File descriptor.  */
	int fd;

	PRINTD("%s: op %.8x - resource %s - device %s - value %s\n",
	       __FUNCTION__,
	       op,
	       resource ? : "No Resource",
	       device ? : "No Dev",
	       value ? : "No value");

	/* Open device.  */
	fd = open(device ? device : DEVICE, O_RDWR);
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
	}

	/* Perform operation.  */
	memory_disasm(start, end, color, fd);

	return EXIT_SUCCESS;
}

int register_disasm(void **action)
{
	*action = (void *)disasm_op;

	return EXIT_SUCCESS;
}
#endif /* DISASM  */
