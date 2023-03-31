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

#define __MEMMAPPER_C__
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <debug_func.h>
#include <io_func.h>
#include <mem_func.h>
#include <pci_func.h>
#include <memmapper.h>

#ifdef DISASM
#define OPTS_LIST "ac:d:f:hi:l:m:o:p:su:w:"
#else
#define OPTS_LIST "ac:d:f:hi:l:m:o:p:sw:"
#endif

static void cursor(int on)
{
	if(on)
		system("setterm -cursor on");
	else
		system("setterm -cursor off");
}

static void usage(void)
{
	/* Banner.  */
	printf("Memory mapper version %s\n" "Written by %s\n" "Released under license %s\n\n", VERSION, AUTHOR, LICENSE);

	/* Help.  */
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

static void sigint_handler(int signo)
{
	cursor(TRUE);
	exit(EXIT_SUCCESS);
}

static struct option long_options[] = {
	{ "ascii", 0, 0, 'a' },
	{ "color", 1, 0, 'c' },
	{ "device", 1, 0, 'd' },
	{ "file", 1, 0, 'f' },
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
	/* getopt related variables.  */
	int option;
	int option_index;
	/* User selected device (if any).  */
	char *device = NULL;
	/* User selected resource (mandatory).  */
	char *resource = NULL;
	/* User selected value.  */
	char *value = NULL;
	/* Current operation.  */
	unsigned long op = FIELD(NO_OP, CMD) | FIELD(READ_ONLY, RW) | FIELD(COLOR_EN, COLOR);
	int (*action)(unsigned long op, char *resource, char *device, char *value);
	int __ret;

	if (argc == 1)
		usage();

	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		fprintf(stderr, "Cannot handle SIGINT!\n");
		exit(EXIT_FAILURE);
	}

	/* Disable cursor.  */
	cursor(FALSE);

	while(1) {
		option = getopt_long(argc, argv, OPTS_LIST, long_options, &option_index);
		if (option == -1)
			break;

		switch (option) {
		case 'a':
			op |= FIELD(ASCII_EN, ASCII);
			break;
		case 'c':
			if(optarg) {
				if (strtoul(optarg, 0 , 0) == 0) {
					op &= (~FIELD(COLOR_EN, COLOR));
				}
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
		case 'd':
			if(optarg) {
				device = strdup(optarg);
				if(!device) {
					printf("Error allocating memory.\n"
							"quitting");
					goto end;
				}
				PRINTD("device: %s\n", device);
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
		case 'f':
			/* File option.  */
			break;
		case 'h':
			usage();
			break;
		case 'i':
			register_io((void **)&action);
			op |= FIELD(IO_OP, CMD);
			goto get_resource;
		case 'p':
			register_pci((void **)&action);
			op |= FIELD(PCI_OP, CMD);
			goto get_resource;
#ifdef DISASM
		case 'u':
			register_disasm((void **)&action);
			op |= FIELD(DISASM_OP, CMD);
			goto get_resource;
#endif
		case 'm':
			register_mem((void **)&action);
			op |= FIELD(MEM_OP, CMD);

		get_resource:
			if (optarg) {
				resource = strdup(optarg);
				if (!resource) {
					printf("Error allocating memory.\n"
					       "quitting");
					goto end;
				}
				PRINTD("resource: %s\n", resource);
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
		case 'l':
			if (optarg) {
				switch(strtoul(optarg, 0, 0)) {
				case 1:
					op |= FIELD(LEN1, LENGTH);
					break;
				case 2:
					op |= FIELD(LEN2, LENGTH);
					break;
				case 4:
					op |= FIELD(LEN4, LENGTH);
					break;
				case 8:
					op |= FIELD(LEN8, LENGTH);
					break;
				default:
					op |= FIELD(LEN4, LENGTH);
					break;
				}
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
		case 'o':
			op |= FIELD(WRITE_ONLY, RW);
			goto get_rw;
		case 'w':
			op |= FIELD(WRITE_READ, RW);

		get_rw:
			if(optarg) {
				value = strdup(optarg);
				if(!value) {
					printf("Error allocating memory.\n"
					       "quitting");
					goto end;
				}
				PRINTD("value: %s\n", value);
			} else {
				printf("optarg is null... ABNORMAL!\n");
				goto end;
			}
			break;
		case 's':
			/* aSynchronous open.  */
			op |= FIELD(OPEN_SYNC, OPEN);
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

	/* Set user identity. */
	if(setuid(0) == -1) {
		printf("You do NOT have root privileges, moron!\n");
		goto end;
	}

	PRINTD("%s: op %#x\n",
	       __FUNCTION__,
	       op);

	/* Perform the operation.  */
	__ret = action(op, resource, device, value);

end:
	free(device);
	free(resource);
	cursor(TRUE);

	return __ret;
}
