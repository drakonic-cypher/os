#ifndef _DISK_H
#define _DISK_H

#include "vga.h"

#define PRIMARY_IO_PORT_BASE 0x1f0
#define PRIMARY_CONTROL_BASE 0x3f6

#define SECONDARY_IO_PORT_BASE 0x170
#define SECONDARY_CONTROL_BASE 0x376

//from io port base

#define DATA_REG 	0x0	//R-W
#define ERROR_REG 	0x1	//R
#define FEATURES_REG 	0x1	//W
#define SEC_COUNT_REG	0x2	//R-W
#define LBA_LO		0x3	//R-W
#define LBA_MID		0x4	//R-W
#define LBA_HI		0x5	//R-W
#define DRIVE_REG	0x6	//R-W
#define STATUS_REG	0x7	//R
#define COMMAND_REG	0x7	//W

//from control base

#define ALT_STATUS_REG	0x0	//R
#define DEVICE_CTRL_REG 0x0	//W
#define DRIVE_ADDR_REG	0x1	//R

//error register

#define ER_AMNF		0x01	//address mark not found
#define ER_TKZNF	0x02	//track zero not found
#define ER_ABRT		0x04	//aborted command
#define ER_MCR		0x08	//media change request
#define ER_IDNF		0x10	//ID not found
#define	ER_MC		0x20	//media changed
#define ER_UNC		0x40	//uncorrectable data error
#define	ER_BBK		0x80	//bad block detected

//drive register

/*

BITS

0-3: 	LBA bits 24-27
4:	drive number
5:	set to 1
6:	1 for CHS, 0 for LBA
7:	set to 1

*/

//status register

#define SR_ERR 		0x01	//indicates error
#define SR_IDX		0x02	//index, set to zero
#define SR_CORR		0x04	//corrected data, set to zero
#define	SR_DRQ		0x08	//set when drive has PIO data to transfer or accept
#define	SR_SRV		0x10	//overlapped mode service request
#define	SR_DF		0x20	//drive fault error
#define	SR_RDY		0x40	//clear when drive spun down/after error set otherwise
#define	SR_BSY		0x80	//drive preparing to send/receive data, wait clear

//device control register

#define DCR_ZRO		0x01	//always set to zero
#define DCR_NEIN	0x02	//stop device from sending interrupts
#define DCR_SRST	0x04	//set, then clear (after 5us) software reset
#define DCR_HOB		0x80	//set to read high order byte of last lba48

//device address register

#define DAR_DS0		0x01	//drive 0 select, clears when 0 selected
#define DAR_DS1		0x02	//drive 1 select, clears when 1 selected
#define DAR_HS		0x1c	//ones complement of current head
#define DAR_WTG		0x40	//write gate, low while writing in progress

uint8_t disk_read (uint8_t sectors, uint32_t linear_block_addr, uint16_t disk_sectors_per_block, uint32_t destination_addr);
uint8_t disk_write (uint8_t sectors, uint32_t linear_block_addr, uint16_t disk_sectors_per_block, uint32_t source_addr);

#endif
