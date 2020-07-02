#include "system.h"
#include "idt.h"

// interrupt descriptor table
static struct idt_descriptor	idt_table [INTERRUPT_NUMBER_LIMIT];

// idtr structure used to help define the cpu's idtr register
static struct idtr				idtr_pointer;

uint8_t idt_install() {

	// set up idtr for processor
	idtr_pointer.limit 	= (uint16_t) (sizeof (struct idt_descriptor) * INTERRUPT_NUMBER_LIMIT - 1 );
	idtr_pointer.base	= (uint32_t) idt_table;
	__asm__ ("lidt [idtr_pointer]");

}


// installs a new interrupt handler, last member is pointer to function returning void
int idt_set_gate (uint32_t i, void (* irq) (), uint16_t sel, uint16_t flags ) {

	if (i > INTERRUPT_NUMBER_LIMIT)
		return 1;

	// get base address of interrupt handler
	uint32_t base 			= (uint32_t) irq;

	// store base address into idt
	idt_table[i].base_lo		=	base & 0xffff;
	idt_table[i].base_hi		=	(base >> 16) & 0xffff;
	idt_table[i].reserved		=	0x0;
	idt_table[i].flags		=	flags;
	idt_table[i].sel		=	sel;

	return	0;
}
