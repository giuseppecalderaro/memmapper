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

#ifndef __PCI_FUNC_H__
#define __PCI_FUNC_H__

#define PCI_CMD_PORT	0x0CF8
#define PCI_DATA_PORT	0x0CFC
#define PCI_ENABLE	0x80000000
#define DUMP		0x652E	/* bus:dev.fn:dump  */

enum pci_config_space {
	VendorID = 	0x00,
	DeviceID = 	0x02,
	CommandReg = 	0x04,
	StatusReg =	0x06,
	RevisionID =	0x08,
	ClassCode = 	0x09,
	CacheLine = 	0x0C,
	LatencyTimer = 	0x0D,
	HeaderType = 	0x0E,
	BIST = 		0x0F,
	BAR0 = 		0x10,
	BAR1 = 		0x14,
	BAR2 =		0x18,
	BAR3 =		0x1C,
	BAR4 =		0x20,
	BAR5 =		0x24,
	CardBusCIS =	0x28,
	SSysVendorID =	0x2C,
	SSysDeviceID =	0x2E,
	ExpansionROM =	0x30,
	Reserved =	0x34,
	IRQLine = 	0x3C,
	IRQPin =	0x3D,
	Min_Gnt =	0x3E,
	Max_Lat =	0x3F,
};

enum Command_Register_Bits {
	Cmd_IO =		0x0001,	/* Enable response in I/O space.  */
	Cmd_Memory =		0x0002, /* Enable response in Memory space.  */
	Cmd_Master =		0x0004, /* Enable bus mastering.  */
	Cmd_Special =		0x0008, /* Enable response to special cycles.  */
	Cmd_Invalidate =	0x0010, /* Use memory write and invalidate.  */
	Cmd_VGA_Palette =	0x0020, /* Enable palette snooping.  */
	Cmd_Parity =		0x0040, /* Enable parity checking.  */
	Cmd_Wait =		0x0080, /* Enable address/data stepping.  */
	Cmd_Serr =		0x0100, /* Enable System Error.  */
	Cmd_Fast_Back =		0x0200, /* Enable back-to-back writes.  */
	Cmd_Intx_Disable =	0x0400, /* INTx emulation disable.  */
};

enum Status_Register_Bits {
	Status_Cap_List =		0x0010, /* Support capability list.  */
	Status_66Mhz =			0x0020, /* Support 66 Mhz PCI 2.1 bus.  */
	Status_UDF =			0x0040, /* Support User Definable Features (obsolete).  */
	Status_Fast_Back =		0x0080, /* Accept fast-back to back.  */
	Status_Parity =			0x0100, /* Detected parity error.  */
	Status_DevSel_Mask =		0x0600,	/* DEVSEL timing.  */
	Status_DevSel_Fast =		0x0000,
	Status_DevSel_Medium =		0x0200,
	Status_DevSel_Slow =		0x0400,
	Status_Sig_Target_Abort =	0x0800, /* Set on target abort.  */
	Status_Rec_Target_Abort =	0x1000, /* Master ack of "  */
	Status_Rec_Master_Abort =	0x2000, /* Set on master abort.  */
	Status_Sig_System_Error =	0x4000, /* Set when we drive SERR.  */
	Status_Detected_Parity =	0x8000, /* Set on parity error.  */
};

#ifndef __PCI_FUNC_C__
#define DECLARE extern
#else
#define DECLARE
#endif

DECLARE int register_pci(void **action);

#undef DECLARE

#endif /* __PCI_FUNC_H__  */
