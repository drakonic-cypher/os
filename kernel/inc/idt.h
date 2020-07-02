#ifndef _IDT_H
#define _IDT_H

struct idt_descriptor {

        // bits 0-16 of interrupt routine (ir) address
        uint16_t                base_lo;

        // code selector in gdt
        uint16_t                sel;

        // reserved, shold be 0
        uint8_t                 reserved;

        // bit flags
        uint8_t                 flags;

        // bits 16-32 of ir address
        uint16_t                base_hi;
} __attribute__ ((__packed__));

// describes the structure for the processors idtr register
struct idtr {

        // size of the interrupt descriptor table (idt)
        uint16_t                limit;

        // base address of idt
        uint32_t                base;
} __attribute__ ((__packed__));

/* This defines what the stack looks like after an ISR was running */
struct regs
{
    uint32_t gs, fs, es, ds;                            /* pushed the segs last */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;    /* pushed by 'pushad' */
    uint32_t int_no, err_code;                          /* our 'push byte #' and ecodes do this */
    uint32_t eip, cs, eflags, useresp, ss;              /* pushed by the processor automatically */ 
};

uint8_t idt_install();
int idt_set_gate (uint32_t i, void (* irq) (), uint16_t sel, uint16_t flags);

#endif
