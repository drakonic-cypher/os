#include "system.h"
#include "memmngr.h"
#include "vga.h"

/* for reading from the I/O ports to get data
*  from devices such as the keyboard. using what is called
*  'inline assembly' in these routines to actually do the work */
uint8_t inportb (uint16_t _port)
{
    uint8_t value;
    __asm__ __volatile__ ("inb %0, %1" : "=a" (value) : "dN" (_port));
    return value;
}

/* write to I/O ports to send bytes to devices. Ie. for changing the textmode cursor
*  position. Inline assembly for the stuff that simply cannot be done in C */
void outportb (uint16_t _port, uint8_t _data)
{
    __asm__ __volatile__ ("outb %0, %1" : : "dN" (_port), "a" (_data));
}

/* for reading from the I/O ports to get data (16 bits) */
uint16_t inportw (uint16_t _port)
{
    uint16_t value;
    __asm__ __volatile__ ("inw %0, %1" : "=a" (value) : "dN" (_port));
    return value;
}

/* write to I/O ports to send bytes to devices (16 bits)*/
void outportw (uint16_t _port, uint16_t _data)
{
    __asm__ __volatile__ ("outw %0, %1" : : "dN" (_port), "a" (_data));
}

bool string_compare (char * string_check, char * string_source, uint8_t length) {

	while (length > 0) {
		if (*string_source != *string_check)
			return false;

		string_check++;
		string_source++;
		length--;
	}

	//the last check compares the null terminating character on string check
	return (*string_check == 0x0);
}

bool test_a20_line () {

	uint8_t * test_addr = (uint8_t *) 0x0;
	uint8_t * wrap_around_addr = (uint8_t *) 0x100000;

	*test_addr = 0xd1;
	*wrap_around_addr = 0x0;

	if (*test_addr == 0xd1)
		return true;
	else
		return false;
}


//Rondo
void play_music () {

	play_sound (494);
	play_sound (440);
	play_sound (415);
	play_sound (440);
	play_sound (523);

}

void kernel_main (char test) {

	gdt_install ();
	isrs_install ();
	filesystem_initialize();
	screen_initialize ();
	vmem_initialize ();
	irq_install ();

	play_music ();

	timer_animate_frame ();

}
