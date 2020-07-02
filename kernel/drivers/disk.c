#include "disk.h"

//28 bit PIO

uint8_t wait_command_clear () {

	uint8_t status;

	do {

		status = inportb (PRIMARY_CONTROL_BASE + ALT_STATUS_REG);

	} while ( status & (SR_DRQ | SR_BSY) );

	if (status & SR_ERR) {
		print_string ("DISK ERROR");
		return 0x1;
	}

	return 0x0;
}

uint8_t wait_poll () {

	uint8_t status;

        do {

                status = inportb (PRIMARY_CONTROL_BASE + ALT_STATUS_REG);

        } while ( (status & SR_BSY) || !(status & SR_DRQ) );

}

void delay_400 () {

	uint8_t status;
        uint8_t index = 0x0;


        while (index < 0x4) {
                status = inportb (PRIMARY_CONTROL_BASE + ALT_STATUS_REG);
                index ++;
        }


}

void set_drive (uint8_t drive, uint8_t lba_24_28) {

//which procedure for drive select ?

	wait_command_clear ();

        //uint8_t drive_addr_byte = 0x0;
        //outportb (PRIMARY_CONTROL_BASE + DRIVE_ADDR_REG, drive_addr_byte);

        uint8_t drive_reg_byte = 0xe0 | (lba_24_28 & 0xf);
        outportb (PRIMARY_IO_PORT_BASE + DRIVE_REG, drive_reg_byte);

//read status five times after drive select (give time for drive to push status onto line)

	delay_400 ();

}

void set_lba (uint32_t sector) {

	outportb (PRIMARY_IO_PORT_BASE + LBA_LO, sector & 0xff);

	outportb (PRIMARY_IO_PORT_BASE + LBA_MID, (sector >> 8) & 0xff);

	outportb (PRIMARY_IO_PORT_BASE + LBA_HI, (sector >> 16) & 0xff);

}

void set_sec_count (uint8_t sectors) {

        outportb (PRIMARY_IO_PORT_BASE + SEC_COUNT_REG, sectors);

}


void read_to_buffer (uint16_t * dest_addr) {

	wait_poll ();

	uint16_t index = 0x0;

        while (index < 0x100) {
		*dest_addr = inportw (PRIMARY_IO_PORT_BASE + DATA_REG);
                dest_addr++;
                index++;
        }

}

uint8_t disk_read (uint8_t sectors, uint32_t linear_block_addr, uint16_t disk_sectors_per_block, uint32_t destination_addr) {

        uint32_t base_sector = linear_block_addr * disk_sectors_per_block;
	uint16_t * dest_addr = (uint16_t *) destination_addr;

//drive set
	set_drive (0x0, base_sector >> 24);

//send null to features register
        outportb (PRIMARY_IO_PORT_BASE + FEATURES_REG, 0x0);

	set_sec_count (sectors);

	set_lba (base_sector);

//READ SECTORS command
	wait_command_clear ();
        outportb (PRIMARY_IO_PORT_BASE + COMMAND_REG, 0x20);

	uint8_t i;

	for (i = 0; i < sectors; i ++) {
		read_to_buffer (dest_addr + i * 0x100);
		delay_400 ();
	}

        return 0x0;
}

void write_from_buffer (uint16_t * src_addr) {

	wait_poll ();

	uint16_t index = 0x0;

        while (index < 0x100) {
		outportw (PRIMARY_IO_PORT_BASE + DATA_REG, *src_addr);
                src_addr++;
                index++;
        }

}

uint8_t disk_write (uint8_t sectors, uint32_t linear_block_addr, uint16_t disk_sectors_per_block, uint32_t source_addr) {

	uint8_t base_sector = linear_block_addr * disk_sectors_per_block;
        uint16_t * src_addr = (uint16_t *) source_addr;

	set_drive (0x0, base_sector >> 24);

//send null to features register
	outportb (PRIMARY_IO_PORT_BASE + FEATURES_REG, 0x0);

	set_sec_count (sectors);

	set_lba (base_sector);

//WRITE SECTORS command
	outportb (PRIMARY_IO_PORT_BASE + COMMAND_REG, 0x30);

	uint8_t i;

	for (i = 0; i < sectors; i++)
		write_from_buffer (src_addr + i * 0x100);

//cache flush
	outportb (PRIMARY_IO_PORT_BASE + COMMAND_REG, 0xe7);

	return 0x0;
}


