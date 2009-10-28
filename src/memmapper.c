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

#define MEMMAPPER_C_
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DISASM
#include <udis86.h>
#endif
#include <unistd.h>

#include "inc/debug_func.h"
#include "inc/io_func.h"
#include "inc/lib_func.h"
#include "inc/mem_func.h"
#include "inc/memmapper.h"
#include "inc/pci_func.h"

void cursor(int on)
{
	if(on)
		system("setterm -cursor on");
	else
		system("setterm -cursor off");
}

void usage(void)
{
	printf("memory mapper help.\n"
	       "./memmapper [--pci | -p] <bus:dev.fn:reg> [--length | -l] <length>\n"
	       "\treads pci configuration space.\n\n"
	       "./memmapper [--pci | -p] <bus:dev.fn:reg> [--length | -l] <length> [--writeread | -w] <value>\n"
	       "\twrites pci configuration space.\n\n"			
	       "./memmapper [--io | -i] <port> [[--length | -l] <length>]\n"
	       "\treads I/O port.\n\n"
	       "./memmapper [--io | -i] <port> [[--length | -l] <length>] [--writeread | -w] <value>\n"
	       "\twrites I/O port.\n\n"
	       "./memmapper [[--device | -d] <device>] [--memory | -m] <address> [[--length | -l] <length>]\n"
	       "\treads physical memory address.\n\n"
	       "./memmapper [[--device | -d] <device>] [--memory | -m] <address> [[--length | -l] <length>] [--writeread | -w] <value>\n"
	       "\twrites value in the specified physical memory address.\n\n"
	       "./memmapper [[--device | -d] <device>] [--memory | -m] <begin>:<end> [[--length | -l] <length>]\n"
	       "\treads physical memory address range.\n\n"
	       "./memmapper [[--device | -d] <device>] [--memory | -m] <begin>:<end> [[--length | -l] <length>] [--writeread | -w] <value>\n"
	       "\twrites value in the specified physical memory address range.\n\n"
	       "./memmapper [[--device | -d] <device>] [--memory | -m] <begin>:<end> [[--length | -l] <length>] [--writeread | -w] <value>:<increment>\n"
	       "\twrites (value + increment) in the specified physical memory address range.\n\n"
#ifdef DISASM
	       "./memmapper [[--device | -d] <device>] [--disassemble | -u] <begin>:<end>\n"
	       "\tdisassemble physical memory address.\n\n"
#endif
	       "options:\n"
	       "[--color | -c] 1 (enables colors (default))\n"
	       "[--color | -c] 0 (disables colors)\n"
	       "[--help | -h] (prints this nice help message)\n"
	       "[--writeonly | -o] <value> (like writeread but it doesn't read after writing).\n"
	       "[--asynchronous | -s] (disables O_SYNC flag in open)\n\n\n");	
	cursor(TRUE);
	exit(EXIT_SUCCESS);
}

void sigint_handler(int signo)
{
	cursor(TRUE);
	exit(EXIT_SUCCESS);
}

static struct option long_options[] = {
	{ "ascii", 0, 0, 'a' },
	{ "color", 1, 0, 'c' },
	{ "device", 1, 0, 'd' },
	{ "help", 0, 0, 'h' },
	{ "memory", 1, 0, 'm'},
	{ "io", 1, 0, 'i'},
	{ "length", 1, 0, 'l' },
	{ "writeonly", 1, 0, 'o' },
	{ "pci", 1, 0, 'p' },
	{ "asynchronous", 0, 0, 's' },
#ifdef DISASM
	{ "disassemble", 1, 0, 'u' },
#endif
	{ "writeread", 1, 0, 'w' },
	{ 0, 0, 0, 0 }
};

int main(int argc, char **argv)
{
	/* File descriptor.  */
	int fd;
	/* getopt related variables.  */
	int option;
	int option_index;
	/* User selected device (if any).  */
	char *device = NULL;
	int mem_open_flags = O_RDWR | O_SYNC;
	/* action to be performed.  */
	int action = 0;
	int increment = 0;
	/* Start - end physical addresses.  */
	unsigned long phys_start = 0;
	unsigned long phys_end = 0;
	/* Default operation length.  */
	int length = -1;
	/* PCI variables.  */
	unsigned char bus, devfn;
	int reg;
	/* To write or not to write, that is the question.  */
	int op = COLORS;
	/* Data (I/O). */
	char *from = NULL;
	char sep;
	char chr[] = ":+";
	unsigned long data = 0;
	int sign = 0;
	int i;

	/* Banner.  */
	printf("Memory mapper version %s\n" "Written by %s\n" "Released under license %s\n\n", VERSION, AUTHOR, LICENSE);
	
	if(argc == 1)
		usage();
	
	if(signal(SIGINT, sigint_handler) == SIG_ERR) {
		fprintf(stderr, "Cannot handle SIGINT!\n");
		exit(EXIT_FAILURE);
	}

	cursor(FALSE);
	while(1) {
		option = getopt_long(argc, argv, "ac:d:hi:l:m:o:p:su:w:", long_options, &option_index);
		if(option == -1)
			break;
			
		switch(option) {
		case 'a':
			op |= ASCII;
			length = 1;
			break;
		case 'c':
			PRINTD("optarg: %s\n", optarg);
			if(optarg) {
				if(hex_encoder(optarg, &sign) == 0)
					op &= (~COLORS);
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
		case 'd':
			PRINTD("optarg: %s\n", optarg);
			if(optarg) {
				device = (char *)malloc(strlen(optarg));
				if(!device) {
					printf("Error allocating memory.\n"
							"quitting");
					goto end;
				}			   
				memset(device, 0, strlen(optarg));
				strncpy(device, optarg, strlen(optarg));
				PRINTD("device: %s\n", device);
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
		case 'h':
			usage();
			break;
		case 'i':
			if(optarg) {
				phys_start = hex_encoder(optarg, &sign);
				action = IO_OP;
				PRINTD("action: %d\n"
						"value: %.4lx\n", action, phys_start);				
			} else {
				printf("optarg is null... ABNORMAL!\n");
			}
			break;
		case 'l':
			if((op & ASCII) || (op & DISASSEMBLE)) {
				length = 1;				
			} else {
				if(optarg) {
					length = hex_encoder(optarg, &sign);
					if(length == 0) {
						printf("length can not be null. Defaulting to 1.\n");
						length = 1;
					}
					PRINTD("length: %d\n", length);
				} else {
					printf("optarg is null... ABNORMAL!\n");
					goto end;
				}				
			}
			break;			
		case 'm':
			if(optarg) {
				for(i = 0; !from && i < 3; i++) {
					from = strchr(optarg, chr[i]);
					if(from)
						sep = from[0];
				}
				from = strsep(&optarg, ":+");			        
				phys_start = hex_encoder(from, &sign);
				if(optarg) {
					phys_end = hex_encoder(optarg, &sign);
					PRINTD("dump enabled\n"
					       "physical start: %lx\n"
					       "sep: %c\n"
					       "physical end: %lx\n",
					       phys_start, sep, phys_end);
				} else {
					PRINTD("dump disabled\n"
					       "physical: %lx\n",
					       phys_start);
				}
				action = MEM_OP;							
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
		case 'p':
			if(optarg) {
				/* Obtain bus, devfn, reg values.  */
				decode_pci_address(optarg, &bus, &devfn, &reg);
				action = PCI_OP;
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
		case 's':
			/* aSynchronous open.  */
			mem_open_flags &= (~O_SYNC);
			break;
#ifdef DISASM
		case 'u':
			if(optarg) { 
				from = strsep(&optarg, ":-");
				phys_start = hex_encoder(from, &sign);
				if(optarg) {
					phys_end = hex_encoder(optarg, &sign);
					PRINTD("dump enabled\n"
					       "physical start: %lx\n"
					       "physical end: %lx\n",
					       phys_start, phys_end);
				} else {
					PRINTD("dump disabled\n"
					       "physical: %lx\n",
					       phys_start);
				}
				action = DISASSEMBLE;
				length = 1;
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
#endif
		case 'w':
			op |= WRITE_READ;
		case 'o':
			if(optarg) {
				from = strsep(&optarg, ":-");
				data = hex_encoder(from, &sign);
				if(optarg) {
					increment = hex_encoder(optarg, &sign);
					increment *= sign;
					PRINTD("incremental write enabled. increment: %d\n", increment);
				} else {
					PRINTD("incremental write disabled\n");
				}				
				op |= WRITE_ONLY;
				PRINTD("op: %d\n"
						"value: %lx\n",
						op, data);				
			} else {
				printf("optarg is null... ABNORMAL!\n");
			}
			break;
		case '?':
			printf("Unknown option specified.\n");
			usage();
			break;
		default:
			/* Nothing to do... ;-)  */
			break;
		}
	}
	
	PRINTD("device: %s\n"
			"action: %d\n"
			"write enabled: 0x%x\n",
			(device ? device : DEVICE), action, op);
	
	/* Set user identity.  */
	if(setuid(0) == -1) {
		printf("You do NOT have root privileges, moron!\n");
		goto end;
	}
	
	switch(action) {
#ifdef DISASM
	case DISASSEMBLE:
		/* Open device.  */
		fd = open(device ? device : DEVICE, mem_open_flags);
		if(fd == -1) {
			printf("Cannot open file descriptor...\n"
					"quitting...\n");
			goto end;
		}
		if(memory_disassemble(phys_start, phys_end, op, fd))
			goto end;
		break;
#endif
	case MEM_OP:
		/* Open device.  */
		fd = open(device ? device : DEVICE, mem_open_flags);
		if(fd == -1) {
			printf("Cannot open file descriptor...\n"
					"quitting...\n");
			goto end;
		}
		if(memory_dumper(phys_start, sep, phys_end, &data, length, increment, op, fd))
			goto end;
		break;
	case IO_OP:
		io_rw(phys_start, &data, length, op);
		break;
	case PCI_OP:
		if(reg == -1) {
			PRINTD("PCI dump operation.\n");
			pci_conf_dump_all(bus, devfn);
		} else {
			PRINTD("PCI operation.\n");
			if(length == -1) {
				printf("You MUST specify length for a PCI operation...\n"
						"quitting...\n");
				goto end;
			}
			pci_rw(bus, devfn, reg, &data, length, op);
		}
		break;
	case 0:
	default:
		usage();
		break;
	}
	
end:
	printf("\n");
	cursor(TRUE);
	free(device);

	return 0;
}
