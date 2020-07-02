#include "filesystem.h"
#include "disk.h"
#include "memmngr.h"

//The copies of the superblock and the group descriptor table are found at blocks 0, 1 and those that are power of 3, 5 and 7

#define DISK_SECTOR_SIZE 0x200
#define BOOT_BLOCK 0x0
#define SUPERBLOCK_BLOCK 0x1
#define BGDT_BLOCK 0x2
#define ROOT_INODE 0x2

#define DISK_BUFFER_ADDR 0x600000
#define DIR_COLOR 0xf0

struct superblock * _superblock;
struct extended_superblock * _extended_superblock;

static uint16_t disk_sectors_per_block = 0x2;           //begin with default value

static uint32_t block_size;
static uint32_t fragment_size;
static uint16_t inode_size;
static uint32_t inodes_per_blockgroup;
static uint32_t inodes_per_block;
static uint32_t blocks_per_blockgroup;
static uint32_t descriptors_per_block;
static uint32_t inode_table_block;
struct dir_entry current_working_dir;

struct inode inode_copy;

uint32_t calculate_inode_blockgroup (uint32_t inode_number) {

        return (inode_number - 1) / inodes_per_blockgroup;

}

uint32_t calculate_inode_index (uint32_t inode_number) {

        return (inode_number - 1) % inodes_per_blockgroup;

}

/*
uint32_t calculate_inode_containing_block (uint32_t inode_index) {

        return (inode_index * inode_size) / block_size;

}
*/

void load_read_superblock () {

        disk_read (0x2, SUPERBLOCK_BLOCK, disk_sectors_per_block, DISK_BUFFER_ADDR);
        _superblock = (struct superblock *) DISK_BUFFER_ADDR;
        _extended_superblock = (struct extended_superblock *) ( _superblock + 0x1 );

        block_size = SIZE_UNIT << _superblock->block_size;
        fragment_size = SIZE_UNIT << _superblock->fragment_size;
        inode_size = _extended_superblock->inode_size;
        inodes_per_block = block_size / inode_size;
        inodes_per_blockgroup = _superblock->inodes_per_bgrp;
        blocks_per_blockgroup = _superblock->blocks_per_bgrp;
        descriptors_per_block = block_size / sizeof(struct block_group_descriptor_table);
        disk_sectors_per_block = block_size / DISK_SECTOR_SIZE;

}

void load_read_bgrp_des_table (uint32_t inode_number) {

        uint32_t inode_blockgroup = calculate_inode_blockgroup (inode_number);
        uint32_t descriptor_table_offset = inode_blockgroup / descriptors_per_block;
        uint32_t table_index = inode_blockgroup % descriptors_per_block;
        uint32_t descriptor_table_block = BGDT_BLOCK + descriptor_table_offset;

        disk_read (disk_sectors_per_block, descriptor_table_block, disk_sectors_per_block, DISK_BUFFER_ADDR);
        struct block_group_descriptor_table * des_table_entry = (struct block_group_descriptor_table *) DISK_BUFFER_ADDR;

        des_table_entry += table_index;

        inode_table_block = des_table_entry->block_addr_inode_table;

}

void read_singly_indirect_block_pointer (uint32_t singly_indirect_block_pointer, uint32_t * dest_index) {

        disk_read (disk_sectors_per_block, singly_indirect_block_pointer, disk_sectors_per_block, DISK_BUFFER_ADDR);
        uint32_t * block_pointer = (uint32_t *) DISK_BUFFER_ADDR;
        uint32_t count = * dest_index;

        while ( *block_pointer && (count < (0x100 + *dest_index)) ) {
                count++;
                disk_read (disk_sectors_per_block, *block_pointer, disk_sectors_per_block, DISK_BUFFER_ADDR + count * block_size);
                block_pointer++;
        }

        *dest_index = count;

}

void read_doubly_indirect_block_pointer (uint32_t doubly_indirect_block_pointer, uint32_t * dest_index) {

        disk_read (disk_sectors_per_block, doubly_indirect_block_pointer, disk_sectors_per_block, DISK_BUFFER_ADDR);
        uint32_t block_pointers_copy [0x100];
        memcpy ((uint8_t *) DISK_BUFFER_ADDR, (uint8_t *) block_pointers_copy, 0x400);

        uint32_t * block_pointer = (uint32_t *) block_pointers_copy;
        uint32_t count = 0;

        while ( *block_pointer && ( count < 0x100 ) ) {
                read_singly_indirect_block_pointer (*block_pointer, dest_index);
                block_pointer++;
                count++;
        }

}

void read_triply_indirect_block_pointer (uint32_t triply_indirect_block_pointer, uint32_t * dest_index) {

        disk_read (disk_sectors_per_block, triply_indirect_block_pointer, disk_sectors_per_block, DISK_BUFFER_ADDR);
        uint32_t block_pointers_copy [0x100];
        memcpy ((uint8_t *) DISK_BUFFER_ADDR, (uint8_t *) block_pointers_copy, 0x400);

        uint32_t * block_pointer = (uint32_t *) block_pointers_copy;
        uint32_t count = 0;

        while ( *block_pointer && ( count < 0x100 ) ) {
                read_doubly_indirect_block_pointer (*block_pointer, dest_index);
                block_pointer++;
                count++;
        }

}

void read_inode (struct inode _inode) {

        uint32_t * block_pointer = & _inode.direct_block_pointer_0;

        uint32_t count = 0;

        while (*block_pointer && (count < 12)) {
                count++;
                disk_read (disk_sectors_per_block, *block_pointer, disk_sectors_per_block, DISK_BUFFER_ADDR + count * block_size);
                block_pointer++;
        }

        if ( _inode.singly_indirect_block_pointer )
                read_singly_indirect_block_pointer (_inode.singly_indirect_block_pointer, & count);

        if ( _inode.doubly_indirect_block_pointer )
                read_doubly_indirect_block_pointer (_inode.doubly_indirect_block_pointer, & count);

        if ( _inode.triply_indirect_block_pointer )
                read_triply_indirect_block_pointer (_inode.triply_indirect_block_pointer, & count);

}

//TODO name length might have to be paired with higher bits
struct dir_entry * read_dir (uint8_t * file) {

        struct dir_entry * _dir_entry = (struct dir_entry *) (DISK_BUFFER_ADDR + block_size);

        while (_dir_entry->inode != 0x0) {
                if (string_compare (file, _dir_entry->name, _dir_entry->name_length_low))
                        return _dir_entry;
                _dir_entry = (struct dir_entry *) ( (uint32_t) _dir_entry + _dir_entry->size );

        }

        return 0x0;

}

uint8_t parse_filesystem_address (uint8_t * address) {

        uint8_t count = 0;

        while ( (*address != 0x0) && (*address != '/') ) {
                address++;
                count++;
        }

        return count;

}

uint8_t * parse_inverse_filesystem_address (uint8_t * address, uint8_t * origin) {

	if ( (address - 0x1) >= origin )
		address--;

	while ( (address > origin) && (*(address-0x1) != '/') ) {
		address--;
	}

	return address;

}

void load_inode (uint32_t inode) {

        load_read_bgrp_des_table (inode);
        uint32_t inode_index = calculate_inode_index (inode);
        struct inode * _inode;
        uint32_t block_index = inode_index / inodes_per_block;
        uint32_t inode_offset = inode_index % inodes_per_block;

        disk_read (disk_sectors_per_block, inode_table_block + block_index, disk_sectors_per_block, DISK_BUFFER_ADDR);

        _inode = (struct inode *) DISK_BUFFER_ADDR;
        _inode += inode_offset;

        memcpy ((uint8_t *) _inode, (uint8_t *) & inode_copy, sizeof(struct inode));

        read_inode (inode_copy);

}

void find_file (uint8_t * name, struct dir_entry * _dir_entry) {

        load_inode (_dir_entry->inode);

        struct dir_entry * dir_entry_result = read_dir (name);

        if ( dir_entry_result )
                * _dir_entry = * dir_entry_result;

        else
                (*_dir_entry).inode = 0x0;

}

void set_dir_entry_root (struct dir_entry * _dir_entry) {

        (*_dir_entry).type_or_name_length_high = DE_DIR;
        (*_dir_entry).inode = ROOT_INODE;
        (*_dir_entry).name[0] = '/';
        (*_dir_entry).name[1] = 0x0;

}

struct dir_entry filesystem_search (uint8_t * file) {

        uint8_t index = 0;
        struct dir_entry _dir_entry;
        set_dir_entry_root (&_dir_entry);
	uint8_t i = 0x0;

        if ( file[index] == '/' ) {
		file++;
        } else {
		uint8_t buffer [SCREEN_HEIGHT * SCREEN_WIDTH];
		copy_string (file, buffer);
		i = string_len (current_working_dir.name);
		copy_string (current_working_dir.name, file);
		copy_string (buffer, file + i);
                _dir_entry = current_working_dir;
        }

	uint8_t * file_origin = file;
	file += i;

	if (*file == 0x0)
		return _dir_entry;

        while ( file[index] && _dir_entry.inode ) {

                index = parse_filesystem_address (file);

                uint8_t dir [index + 0x1];
                dir[index] = 0x0;

                uint8_t count = index;

                do {
                        count--;
                        dir[count] = file[count];

                } while (count != 0);

//must now check whether dir value is equal to . or .. and edit file pointer accordingly

		if (string_compare(dir, ".", 0x1)) {

			uint8_t offset = ( (file[index] == '/') ? (index + 0x1) : index );
			copy_string (file + offset, file);

			index = 0x0;
			continue;

		} else if (string_compare (dir, "..", 0x2)) {

			find_file (dir, &_dir_entry);

			uint8_t * source_address = ( (file[index] == '/') ? (file + index + 0x1) : (file + index) );
			file = parse_inverse_filesystem_address (file, file_origin);

			copy_string (source_address, file);
			index = 0x0;
			continue;

		}

                find_file (dir, &_dir_entry);

                if ( (file[index] == '/') ) {
                        if (_dir_entry.type_or_name_length_high != DE_DIR) {
				_dir_entry.inode = 0x1;
                        	return _dir_entry;
			}

			file++;
                }

		file += index;
		index = 0x0;

        }

	if ( (_dir_entry.inode) && (*(--file) != '/') && (_dir_entry.type_or_name_length_high == DE_DIR) ) {
                 *(++file) = '/';
                 *(++file) = 0x0;
        }

        return _dir_entry;

}

void print_dir (uint32_t inode) {

        load_inode (inode);

        struct dir_entry * _dir_entry = (struct dir_entry *) (DISK_BUFFER_ADDR + block_size);

        print_string ( "   " );
        while (_dir_entry->inode != 0x0) {
                if (_dir_entry->type_or_name_length_high == DE_DIR)
                        print_string_color (_dir_entry->name, DIR_COLOR);
                else
                        print_string (_dir_entry->name);
                print_tab ();
                _dir_entry = (struct dir_entry *) ( (uint32_t) _dir_entry + _dir_entry->size );

        }

}

void change_dir (struct dir_entry _dir_entry, uint8_t * filesystem_address) {

	copy_string (filesystem_address, _dir_entry.name);
	current_working_dir = _dir_entry;

}

void mkdir (uint8_t * address) {

}

void rmdir (uint8_t * address) {

}

uint8_t filesystem_initialize () {

        load_read_superblock ();

        set_dir_entry_root (&current_working_dir);
}
