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

#ifndef MEM_FUNC_H_
#define MEM_FUNC_H_

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

int memory_rw(unsigned long physical, unsigned long *data, int length, int increment, int op, int fd);
int memory_dumper(unsigned long from, char sep, unsigned long to, unsigned long *data, int length, int increment, int op, int fd);

#ifdef DISASM
int memory_disassemble(unsigned long from, unsigned long to, int op, int fd);
#endif

#endif /*MEM_FUNC_H_*/
