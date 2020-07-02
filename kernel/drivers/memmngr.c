#include "memmngr.h"

void memcpy (uint8_t * source, uint8_t * destination, uint32_t size) {

        while (size > 0) {
                *destination = *source;
                destination++;
                source++;
                size--;
        }

}

void memset (uint16_t value, uint16_t * destination, uint32_t size) {

        while (size > 0) {
                *destination = value;
                destination ++;
                size--;
        }
}

void pt_entry_add_attribute (pt_entry * page_table_entry, uint32_t attribute) {

	(*page_table_entry) |= attribute;

}

void pt_entry_del_attribute (pt_entry * page_table_entry, uint32_t attribute) {

	(*page_table_entry) &= ~attribute;

}

void pt_entry_set_frame (pt_entry * page_table_entry, uint32_t physical_addr) {

	(*page_table_entry) |= (physical_addr & 0xfffff000);

}

bool pt_entry_is_present (pt_entry page_table_entry) {

	return ( (page_table_entry & PTE_PRESENT) == PTE_PRESENT);

}

bool pt_entry_is_writable (pt_entry page_table_entry) {

	return ( (page_table_entry & PTE_WRITABLE) == PTE_WRITABLE);

}

void pd_entry_add_attribute (pd_entry * page_dir_entry, uint32_t attribute) {

	(*page_dir_entry) |= attribute;

}

void pd_entry_del_attribute (pd_entry * page_dir_entry, uint32_t attribute) {

	(*page_dir_entry) &= ~attribute;

}

void pd_entry_set_frame (pd_entry * page_dir_entry, uint32_t physical_addr) {

	(*page_dir_entry) |= (physical_addr & 0xfffff000);

}

void pd_entry_enable_global (pd_entry * page_dir_entry) {

	(*page_dir_entry) |= PDE_CPU_GLOBAL;

}

bool pd_entry_is_present (pd_entry page_dir_entry) {

	return ( (page_dir_entry & PDE_PRESENT) == PDE_PRESENT);

}

bool pd_entry_is_user (pd_entry page_dir_entry) {

	return ( (page_dir_entry & PDE_USER) == PDE_USER);

}

bool pd_entry_is_4mb (pd_entry page_dir_entry) {

	return ( (page_dir_entry & PDE_4MB) == PDE_4MB);

}

bool pd_entry_is_writable (pd_entry page_dir_entry) {

	return ( (page_dir_entry & PDE_WRITABLE) == PDE_WRITABLE);

}

//malloc_page, set_bitmap_page & free_bitmap_page administers the bitmap

uint8_t set_bitmap_page (uint32_t address) {

	uint32_t page_byte = (address / PAGE_SIZE) / 8;
	uint8_t byte_offset = (address / PAGE_SIZE) % 8;

        uint8_t * bitmap_address = (uint8_t *) (MEM_BITMAP_ORIGIN + page_byte);

	if ( *bitmap_address & (0x1 << byte_offset) )
		return 0x1;

        *bitmap_address |= (0x1 << byte_offset);

	return 0x0;

}

uint8_t free_bitmap_page (uint32_t address) {

        uint32_t page_byte = (address / PAGE_SIZE) / 8;
        uint8_t byte_offset = (address / PAGE_SIZE) % 8;

	uint8_t * bitmap_address = (uint8_t *) (MEM_BITMAP_ORIGIN + page_byte);
        *bitmap_address &= ~(0x1 << byte_offset);

	return 0x0;

}

uint32_t malloc_page () {

        uint32_t page_byte = 0x0;
        uint8_t byte_offset = 0x0;

	uint8_t * bitmap_pointer = (uint8_t *) MEM_BITMAP_ORIGIN;

	while ( (*bitmap_pointer == 0xff) && (page_byte < MEM_BITMAP_SIZE) ) {
		page_byte++;
		bitmap_pointer++;
	}

	if (page_byte >= MEM_BITMAP_SIZE)
		return 0x0;

	uint8_t byte_check = 0x1;

	while ( (*bitmap_pointer & byte_check) && (byte_check < 0x100) ) {

		byte_offset++;
		byte_check <<= 0x1;

	}

	uint32_t address = (page_byte * 8 + byte_offset) * PAGE_SIZE;

	if (set_bitmap_page (address))
		return 0x0;

	return address;

}


//TODO malloc and free should allocate and free memory in linked list form 
uint32_t malloc (uint32_t size) {

}

uint8_t free (uint32_t address) {

}

void mem_bitmap_initialize () {

	memset (0x0, (uint16_t *) MEM_BITMAP_ORIGIN, MEM_BITMAP_SIZE / 0x2);
}


uint8_t vmem_initialize () {

//keep a bitmap of the pages in use
//enable paging

	mem_bitmap_initialize ();

	uint32_t page_table_origin = VM_DATA_STRUCTURES;
	uint16_t page_dir_attributes = 0x0;
	uint32_t index = 0x0;

	while (index < PAGE_TABLES_PER_DIR) {
		pd_entry_add_attribute (&(page_dir_kernel.page_dir_entry[index]), page_dir_attributes);
		pd_entry_set_frame (&(page_dir_kernel.page_dir_entry[index]), page_table_origin + index * PAGES_PER_PAGE_TABLE);
		index++;
	}

	struct page_table * page_table = (struct page_table *) VM_DATA_STRUCTURES;

	index = 0x0;

	uint32_t page_address = 0x0;
	uint16_t page_attributes = 0x0;

	while (index < PAGE_TABLES_PER_DIR) {

		uint32_t offset = 0x0;

		while (offset < PAGES_PER_PAGE_TABLE) {
			pt_entry_add_attribute (page_table->page_table_entry + offset, page_attributes);
			pt_entry_set_frame (page_table->page_table_entry + offset, page_address);
			page_address += 0x1000;
			offset++;
		}

		page_table++;
		index ++;

	}

}
