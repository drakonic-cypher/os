#ifndef _GDT_H
#define _GDT_H

// gdt descriptor. A gdt descriptor defines the properties of a specific  memory block and permissions.

struct gdt_descriptor {

        // bits 0-15 of segment limit
        uint16_t                limit;

        // bits 0-23 of base address
        uint16_t                base_low;
        uint8_t                 base_mid;

        // descriptor bit flags. Set using bit masks
        uint8_t                 access;

        uint8_t                 granularity;

        // bits 24-32 of base address
        uint8_t                 base_hi;
} __attribute__ ((__packed__));

struct gdtr {

        //size of gdt
        uint16_t                limit;

        //base address of gdt
        uint32_t                base;

} __attribute__ ((__packed__));

#endif
