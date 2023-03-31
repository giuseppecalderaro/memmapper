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

#ifndef __MEM_FUNC_H__
#define __MEM_FUNC_H__

#include <sys/mman.h>
#include <memmapper.h>
#define DEVICE "/dev/mem"
#define DELETE "\b"

static inline unsigned char readb(void *address)
{
	return *(volatile unsigned char *)address;
}

static inline unsigned short int readw(void *address)
{
	return *(volatile unsigned short int *)address;
}

static inline unsigned int readdw(void *address)
{
	return *(volatile unsigned int *)address;
}

static inline unsigned long readl(void *address)
{
	return *(volatile unsigned long *)address;
}

static inline void writeb(void *address, unsigned char data)
{
	*(volatile unsigned char *)address = data;
}

static inline void writew(void *address, unsigned short int data)
{
	*(volatile unsigned short int *)address = data;
}

static inline void writedw(void *address, unsigned int data)
{
	*(volatile unsigned int *)address = data;
}

static inline void writel(void *address, unsigned long data)
{
	*(volatile unsigned long *)address = data;
}

static int memory_rw(unsigned long physical, unsigned long *data, int length, int increment, int write, int fd)
{
	void *address;
	unsigned long read_data = 0;
	int pagesize;
	unsigned long page_mask;

	pagesize = sysconf(_SC_PAGESIZE);
	if(pagesize == -1) {
		perror("sysconf");
		return EXIT_FAILURE;
	}
	page_mask = (~(pagesize - 1));

	address = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, physical & page_mask);
	if(address == MAP_FAILED) {
		printf("Cannot mmap file descriptor.\n"
				"quitting...");
		return EXIT_FAILURE;
	}

	if(write) {
		switch(length) {
		case 1:
			*data &= 0xFF;
			writeb(address + (physical & (~page_mask)), *data);
			if(write & WRITE_READ) {
				read_data = readb(address + (physical & (~page_mask)));
			} else {
				read_data = *data;
			}
			break;
		case 2:
			*data &= 0xFFFF;
			writew(address + (physical & (~page_mask)), *data);
			if(write & WRITE_READ) {
				read_data = readw(address + (physical & (~page_mask)));
			} else {
				read_data = *data;
			}
			break;
		case 4:
			*data &= 0xFFFFFFFF;
			writedw(address + (physical & (~page_mask)), *data);
			if(write & WRITE_READ) {
				read_data = readdw(address + (physical & (~page_mask)));
			} else {
				read_data = *data;
			}
			break;
		case 8:
		default:
			writel(address + (physical & (~page_mask)),*data);
			if(write & WRITE_READ) {
				read_data = readl(address + (physical & (~page_mask)));
			} else {
				read_data = *data;
			}
			break;
		}
	} else {
		switch(length) {
		case 1:
			*data = readb(address + (physical & (~page_mask)));
			break;
		case 2:
			*data = readw(address + (physical & (~page_mask)));
			break;
		case 4:
			*data = readdw(address + (physical & (~page_mask)));
			break;
		case 8:
		default:
			*data = readl(address + (physical & (~page_mask)));
			break;
		}
	}
	munmap(address, pagesize);

	return EXIT_SUCCESS;
}

#ifndef __MEM_FUNC_C__
#define DECLARE extern
#else
#define DECLARE
#endif

DECLARE int register_mem(void **action);

#undef DECLARE

#endif /* __MEM_FUNC_H__  */
