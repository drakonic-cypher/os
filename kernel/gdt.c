#include "system.h"
#include "gdt.h"

struct gdt_descriptor    gdt_table [DESCRIPTOR_NUMBER_LIMIT];

struct gdtr              gdtr_pointer;

void gdt_descriptor_set (uint32_t i, uint32_t limit, uint32_t base, uint8_t access, uint8_t granularity) {

	struct gdt_descriptor gdt_des;

	gdt_des.limit = limit;
	gdt_des.base_low = (base & 0xffff);
	gdt_des.base_mid = (base >> 16) & 0xff;
	gdt_des.granularity = (limit >> 16) & 0x0f;
	gdt_des.granularity |= granularity & 0xf0;
	gdt_des.access = access;
	gdt_des.base_hi = (base >> 24) & 0xff;

	gdt_table[i] = gdt_des;
}

void gdtr_pointer_set (struct gdtr * gdtr_pointer, struct gdt_descriptor * gdt_array) {

	gdtr_pointer->limit = (uint16_t) ((sizeof (struct gdt_descriptor) * DESCRIPTOR_NUMBER_LIMIT) - 1);
	gdtr_pointer->base = (uint32_t) gdt_array;

}

uint8_t gdtr_install () {

	gdtr_pointer_set (& gdtr_pointer, gdt_table);
	gdt_flush();
}

uint8_t gdt_install() {

	//null descriptor
	gdt_descriptor_set (0x0, 0x0, 0x0, 0x0, 0x0);
	//code descriptor
	gdt_descriptor_set (0x1, 0xffffffff, 0x0, 0x9a, 0xcf);
	//data descriptor
	gdt_descriptor_set (0x2, 0xffffffff, 0x0, 0x92, 0xcf);

	gdtr_install();

	return 0;
}
