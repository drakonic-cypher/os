#include "system.h"
#include "idt.h"

/* These are function prototypes for all of the exception
*  handlers: The first 32 entries in the IDT are reserved
*  by Intel, and are designed to service exceptions! */
extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

unsigned char * exception_messages[32] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void fault_handler(struct regs *r)
{
    /* Is this a fault whose number is from 0 to 31? */
    	if (r->int_no < 32) {
        /* Display the description for the Exception that occurred.*/
        for (;;);
    }
}



uint8_t isrs_install () {
    idt_set_gate(0, _isr0, 0x08, 0x8E);
    idt_set_gate(1, _isr1, 0x08, 0x8E);
    idt_set_gate(2, _isr2, 0x08, 0x8E);
    idt_set_gate(3, _isr3, 0x08, 0x8E);
    idt_set_gate(4, _isr4, 0x08, 0x8E);
    idt_set_gate(5, _isr5, 0x08, 0x8E);
    idt_set_gate(6, _isr6, 0x08, 0x8E);
    idt_set_gate(7, _isr7, 0x08, 0x8E);
    idt_set_gate(8, _isr8, 0x08, 0x8E);
    idt_set_gate(9, _isr9, 0x08, 0x8E);
    idt_set_gate(10, _isr10, 0x08, 0x8E);
    idt_set_gate(11, _isr11, 0x08, 0x8E);
    idt_set_gate(12, _isr12, 0x08, 0x8E);
    idt_set_gate(13, _isr13, 0x08, 0x8E);
    idt_set_gate(14, _isr14, 0x08, 0x8E);
    idt_set_gate(15, _isr15, 0x08, 0x8E);
    idt_set_gate(16, _isr16, 0x08, 0x8E);
    idt_set_gate(17, _isr17, 0x08, 0x8E);
    idt_set_gate(18, _isr18, 0x08, 0x8E);
    idt_set_gate(19, _isr19, 0x08, 0x8E);
    idt_set_gate(20, _isr20, 0x08, 0x8E);
    idt_set_gate(21, _isr21, 0x08, 0x8E);
    idt_set_gate(22, _isr22, 0x08, 0x8E);
    idt_set_gate(23, _isr23, 0x08, 0x8E);
    idt_set_gate(24, _isr24, 0x08, 0x8E);
    idt_set_gate(25, _isr25, 0x08, 0x8E);
    idt_set_gate(26, _isr26, 0x08, 0x8E);
    idt_set_gate(27, _isr27, 0x08, 0x8E);
    idt_set_gate(28, _isr28, 0x08, 0x8E);
    idt_set_gate(29, _isr29, 0x08, 0x8E);
    idt_set_gate(30, _isr30, 0x08, 0x8E);
    idt_set_gate(31, _isr31, 0x08, 0x8E);

    idt_install();
}
