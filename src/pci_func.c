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

#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <debug_func.h>
#include <lib_func.h>
#include <memmapper.h>
#include <pci_func.h>

/* PCI address decoder.  */
static int decode_pci_address(const char *pci_dev, unsigned char *bus, unsigned char *devfn, int *reg)
{
	int sign = 0;
	char *bus_string;
	char *dev_string;
	char *fn_string;
	char *reg_string;

	/* pci_dev[2] = pci_dev[5] = pci_dev[7] = pci_dev[strlen(pci_dev)] = 0; */
	reg_string = (char *)pci_dev;
	bus_string = strsep(&reg_string, ":");
	fn_string = strsep(&reg_string, ":");
	dev_string = strsep(&fn_string, ".");
	*reg *= sign;

	*bus = hex_encoder(bus_string, &sign);
	*devfn = (hex_encoder(dev_string, &sign) << 3) | hex_encoder(fn_string, &sign);
	*reg = hex_encoder(reg_string, &sign);

	PRINTD("address decoded.\n"
			"bus: %x\n"
			"devfn: %x\n"
			"reg: %x\n",
			*bus, *devfn, *reg);
}

/* PCI configuration register read.  */
static unsigned int pci_conf_read(unsigned char bus, unsigned char devfn, unsigned char reg, int len)
{
	unsigned int data;

	outl(PCI_ENABLE | (bus << 16) | (devfn << 8) | (reg & ~3), PCI_CMD_PORT);

	switch(len) {
	case 1:
		data = inb(PCI_DATA_PORT + (reg & 3));
		break;
	case 2:
		data = inw(PCI_DATA_PORT + (reg & 2));
		break;
	case 4:
		data = inl(PCI_DATA_PORT);
		break;
	default:
		printf("Lenght should be 1, 2, 4 bytes.\n");
		data = 0;
		break;
	}

	return data;
}

/* PCI configuration register write.  */
static void pci_conf_write(unsigned char bus, unsigned char devfn, unsigned char reg, int len, unsigned int data)
{
	outl(PCI_ENABLE | (bus << 16) | (devfn << 8) | (reg & ~3), PCI_CMD_PORT);

	switch(len) {
	case 1:
		outb((unsigned char)data, PCI_DATA_PORT + (reg & 3));
		break;
	case 2:
		outw((unsigned short int)data, PCI_DATA_PORT + (reg & 2));
		break;
	case 4:
		outl(data, PCI_DATA_PORT);
		break;
	}
}

/* PCI configuration space dumper.  */
static void pci_conf_dump_all(unsigned char bus, unsigned char devfn)
{
	unsigned long data = 0;

	/* Set userspace permission on I/O ports.  */
	iopl(3);

	/* Read VendorID - DeviceID.  */
	data = pci_conf_read(bus, devfn, VendorID, 4);
	PRINTD("Read data: 0x%lx\n", data);
	printf("Vendor ID = 0x%.4lx\n"
			"Device ID = 0x%.4lx\n", data & 0xffff, data >> 16);

	/* Read Command Register - Status Register.  */
	data = pci_conf_read(bus, devfn, CommandReg, 4);
	PRINTD("Read data: 0x%lx\n", data);
	printf("Command Register = 0x%.4lx\n"
			"Status Register = 0x%.4lx\n", data & 0xffff, data >> 16);

	/* Read Revision ID - Class Code.  */
	data = pci_conf_read(bus, devfn, RevisionID, 4);
	PRINTD("Read data: 0x%lx\n", data);
	printf("Revision ID = 0x%.2lx\n"
			"Class Code = 0x%.4lx\n", data & 0xff, data >> 16);

	/* Read Cache Line - Latency Timer - Header Type - BIST.  */
	data = pci_conf_read(bus, devfn, CacheLine, 4);
	PRINTD("Read data: 0x%lx\n", data);
	printf("Cache Line = 0x%.2lx\n"
			"Latency Timer = 0x%.2lx\n"
			"Header Type = 0x%.2lx\n"
			"BIST = 0x%.2lx\n",
			data & 0xff, (data >> 8) & 0xff, (data >> 16) & 0xff,(data >> 24) & 0xff);

	/* Read BARs.  */
	printf("BAR0 = 0x%.8x\n", pci_conf_read(bus, devfn, BAR0, 4));
	printf("BAR1 = 0x%.8x\n", pci_conf_read(bus, devfn, BAR1, 4));
	printf("BAR2 = 0x%.8x\n", pci_conf_read(bus, devfn, BAR2, 4));
	printf("BAR3 = 0x%.8x\n", pci_conf_read(bus, devfn, BAR3, 4));
	printf("BAR4 = 0x%.8x\n", pci_conf_read(bus, devfn, BAR4, 4));
	printf("BAR5 = 0x%.8x\n", pci_conf_read(bus, devfn, BAR5, 4));

	/* Read CardBus CIS Pointer.  */
	printf("CardBus CIS Pointer = 0x%.8x\n", pci_conf_read(bus, devfn, CardBusCIS, 4));

	/* Read SubSystemVendorID - SubSystemDeviceID.  */ 
	data = pci_conf_read(bus, devfn, SSysVendorID, 4);
	PRINTD("Read data: 0x%.4lx\n", data);
	printf("SubSystem Vendor ID = 0x%.4lx\n"
			"SubSystem Device ID = 0x%.4lx\n", data & 0xffff, data >> 16);

	/* Read Expansion ROM Base Address.  */
	printf("Expansion ROM Base Address = 0x%.8x\n", pci_conf_read(bus, devfn, ExpansionROM, 2));

	/* Read IRQ Line - IRQ Pin - Min_Gnt - Max_Lat.  */
	data = pci_conf_read(bus, devfn, IRQLine, 4);
	printf("IRQ Line = 0x%.2lx\n"
			"IRQ Pin = 0x%.2lx\n"
			"Min_Gnt = 0x%.2lx\n"
			"Max_Lat = 0x%.2lx\n",
			data & 0xff, (data >> 8) & 0xff, (data >> 16) & 0xff,(data >> 24) & 0xff);	
}

static void pci_rw(unsigned char bus, unsigned char devfn, unsigned char reg, unsigned long data, int write, int color_en, int length)
{
	unsigned long read_data;
	const char *color;

	/* Set userspace permission on I/O ports.  */
	iopl(3);

	if(write) {
		PRINTD("PCI write operation.\n");
		if(color_en)
			printf("Writing data: " "%s" "0x%lx" "%s" "\n", colors[yellow], data, colors[default_color]);
		else
			printf("Writing data: 0x%lx\n", data);

		pci_conf_write(bus, devfn, reg, length, data);
	}

	if((write == READ_ONLY) || (write == WRITE_READ)) {
		PRINTD("PCI read operation.\n");
		read_data = pci_conf_read(bus, devfn, reg, length);
		if(color_en) {
			color = colors[light_cyan];
			if(write == WRITE_READ) {
				if(read_data != data) {
					color = colors[red];
				} else {
					color = colors[light_green];
				}
			}
			printf("Read data: " "%s" "0x%lx" "%s" "\n", color, read_data, colors[default_color]);
		} else {
			printf("Read data: 0x%lx\n", read_data);
		}
	}
}

static int pci_op(unsigned long op, const char *resource, const char *device, const char *value)
{
	unsigned char bus;
	unsigned char devfn;
	int reg;
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

	/* Decoding of resource string.  */
	decode_pci_address(resource, &bus, &devfn, &reg);

	/* Value.  */
	if(value) {
		data = hex_encoder(value, 0);
	}

	PRINTD("%s: bus %.4x - devfn %.4x - reg %.8x - value %.8x - length %d - write %d\n",
	       __FUNCTION__,
	       bus,
	       devfn,
	       reg,
	       data,
	       length,
	       write);

	/* Perform PCI operation.  */
	pci_rw(bus, devfn, reg, data, write, color, length);

	return EXIT_SUCCESS;
}

int register_pci(void **action)
{
	*action = (void *)pci_op;

	return EXIT_SUCCESS;
}
