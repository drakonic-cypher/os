#ifndef _MEMMNGR_H
#define _MEMMNGR_H

#include "system.h"
#include "vga.h"

enum PTE_FLAGS {

	PTE_PRESENT			=	1,		//00000000000000000000000000000001
	PTE_WRITABLE			=	2,		//00000000000000000000000000000010
	PTE_USER			=	4,		//00000000000000000000000000000100
	PTE_WRITETHROUGH		=	8,		//00000000000000000000000000001000
	PTE_NOT_CACHEABLE		=	0x10,		//00000000000000000000000000010000
	PTE_ACCESSED			=	0x20,		//00000000000000000000000000100000
	PTE_DIRTY			=	0x40,		//00000000000000000000000001000000
	PTE_PAT				=	0x80,		//00000000000000000000000010000000
	PTE_CPU_GLOBAL			=	0x100,		//00000000000000000000000100000000
	PTE_LV4_GLOBAL			=	0x200,		//00000000000000000000001000000000
   	PTE_FRAME			=	0x7FFFF000 	//11111111111111111111000000000000
};

enum PDE_FLAGS {

	PDE_PRESENT			=	1,		//00000000000000000000000000000001
	PDE_WRITABLE			=	2,		//00000000000000000000000000000010
	PDE_USER			=	4,		//00000000000000000000000000000100
	PDE_PWT				=	8,		//00000000000000000000000000001000
	PDE_PCD				=	0x10,		//00000000000000000000000000010000
	PDE_ACCESSED			=	0x20,		//00000000000000000000000000100000
	PDE_DIRTY			=	0x40,		//00000000000000000000000001000000
	PDE_4MB				=	0x80,		//00000000000000000000000010000000
	PDE_CPU_GLOBAL			=	0x100,		//00000000000000000000000100000000
	PDE_LV4_GLOBAL			=	0x200,		//00000000000000000000001000000000
   	PDE_FRAME			=	0x7FFFF000 	//11111111111111111111000000000000
};

#define PAGE_TABLES_PER_DIR 	0x400
#define PAGES_PER_PAGE_TABLE 	0x400
#define PAGE_SIZE		0x1000

#define VM_DATA_STRUCTURES	0x200000

#define MEM_BITMAP_ORIGIN	0x600000
#define MEM_BITMAP_SIZE		0x20000

//page table entry
typedef uint32_t pt_entry;

extern void pt_entry_add_attribute (pt_entry * page_table_entry, uint32_t attribute);
extern void pt_entry_del_attribute (pt_entry * page_table_entry, uint32_t attribute);
extern void pt_entry_set_frame (pt_entry * page_table_entry, uint32_t physical_addr);
extern bool pt_entry_is_present (pt_entry page_table_entry);
extern bool pt_entry_is_writable (pt_entry page_table_entry);

struct page_table {
	pt_entry page_table_entry [PAGES_PER_PAGE_TABLE];
};

//page dir entry
typedef uint32_t pd_entry;

extern void pd_entry_add_attribute (pd_entry * page_dir_entry, uint32_t attribute);
extern void pd_entry_del_attribute (pd_entry * page_dir_entry, uint32_t attribute);
extern void pd_entry_set_frame (pd_entry * page_dir_entry, uint32_t physical_addr);
extern void pd_entry_enable_global (pd_entry * page_dir_entry);
extern bool pd_entry_is_present (pd_entry page_dir_entry);
extern bool pd_entry_is_user (pd_entry page_dir_entry);
extern bool pd_entry_is_4mb (pd_entry page_dir_entry);
extern bool pd_entry_is_writable (pd_entry page_dir_entry);

struct page_dir {
	pd_entry page_dir_entry [PAGE_TABLES_PER_DIR];
};

struct page_dir page_dir_kernel;

void memcpy (uint8_t * source, uint8_t * destination, uint32_t size);
void memset (uint16_t value, uint16_t * destination, uint32_t size);

#endif
