#include "system.h"
#include "idt.h"
#include "timer.h"
#include "keyboard.h"

/* These are ISRs that point to a special IRQ handler
*  instead of the regular 'fault_handler' function */

extern void _irq0 ();
extern void _irq1 ();
extern void _irq2 ();
extern void _irq3 ();
extern void _irq4 ();
extern void _irq5 ();
extern void _irq6 ();
extern void _irq7 ();
extern void _irq8 ();
extern void _irq9 ();
extern void _irq10 ();
extern void _irq11 ();
extern void _irq12 ();
extern void _irq13 ();
extern void _irq14 ();
extern void _irq15 ();


/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
void * irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler (uint8_t irq, void (* handler) (struct regs * r)) {
	irq_routines[irq] = handler;
}

void irq_uninstall_handler (uint8_t irq) {
	irq_routines[irq] = 0;
}

void irq_remap (void) {

	//0x20 is the command register for PIC1
	//0xA0 is the command register for PIC2
	//0x21 is the data register for PIC1
	//0xA1 is the data register for PIC2
	uint8_t pic1_mask = inportb(0x21);
	uint8_t pic2_mask = inportb(0xA1);
	outportb(0x20, 0x11);  		//restart PIC1
	outportb(0xA0, 0x11);  		//restart PIC2
	outportb(0x21, 0x20);  		//PIC1 start at 32
	outportb(0xA1, 0x28);  		//PIC2 start at 40
	outportb(0x21, 0x04);  		//setup cascading
	outportb(0xA1, 0x02);
	outportb(0x21, 0x01); 		//done
	outportb(0xA1, 0x01);
	outportb(0x21, pic1_mask);  	//restore masks
	outportb(0xA1, pic2_mask);

}

/* Each of the IRQ ISRs point to this function, rather than
*  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
*  to be told when you are done servicing them, so need
*  to send them an "End of Interrupt" command (0x20). There
*  are two 8259 chips: The first exists at 0x20, the second
*  exists at 0xA0. If the second controller (an IRQ from 8 to
*  15) gets an interrupt, need to acknowledge the
*  interrupt at BOTH controllers, otherwise, only send
*  an EOI command to the first controller. If not
*  the PIC won't raise any more IRQs */
void irq_handler (struct regs *r)
{
    /* This is a blank function pointer */
    void (*handler) (struct regs *r);

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[r->int_no - 32];

    if (handler) {
        handler(r);
    }

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40) {
        outportb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outportb(0x20, 0x20);
}

void install_handlers () {
        irq_install_handler (0x0, handler_timer);
	irq_install_handler (0x1, handler_keyboard);
}


uint8_t irq_install () {

        irq_remap ();

        idt_set_gate (32, _irq0, 0x8, 0x8e);
        idt_set_gate (33, _irq1, 0x8, 0x8e);
        idt_set_gate (34, _irq2, 0x8, 0x8e);
        idt_set_gate (35, _irq3, 0x8, 0x8e);
        idt_set_gate (36, _irq4, 0x8, 0x8e);
        idt_set_gate (37, _irq5, 0x8, 0x8e);
        idt_set_gate (38, _irq6, 0x8, 0x8e);
        idt_set_gate (39, _irq7, 0x8, 0x8e);
        idt_set_gate (40, _irq8, 0x8, 0x8e);
        idt_set_gate (41, _irq9, 0x8, 0x8e);
        idt_set_gate (42, _irq10, 0x8, 0x8e);
        idt_set_gate (43, _irq11, 0x8, 0x8e);
        idt_set_gate (44, _irq12, 0x8, 0x8e);
        idt_set_gate (45, _irq13, 0x8, 0x8e);
        idt_set_gate (46, _irq14, 0x8, 0x8e);
        idt_set_gate (47, _irq15, 0x8, 0x8e);

	timer_setup();
        install_handlers();
	inportb (0x60); //clear keyboard buffer before restoring interrupts
	__asm__ ("sti");

}

