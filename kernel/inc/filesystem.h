#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "system.h"

//The Superblock is always located at byte 1024 on disk and is 1024 bytes in size
#define SUPERBLOCK_ADDR 0x400
#define SUPERBLOCK_SIZE 0x400

//Inode and Block base size
#define SIZE_UNIT 0x400

//FS states
#define FILESYSTEM_CLEAN 0x1
#define FILESYSTEM_ERROR 0x2

//Error Handling Methods (action to take)
#define IGNORE_ERROR 0x1
#define REMOUNT_AS_READ_ONLY 0x2
#define KERNEL_PANIC 0x3

//Creator OS Ids
#define LINUX_ID 0x0
#define GNU_HURD_ID 0x1
#define MASIX_ID 0x2
#define FREEBSD_ID 0x3
#define OTHER_LITES_ID 0x4

struct superblock {

	uint32_t num_inodes;
	uint32_t num_blocks;
	uint32_t superuser_reserved_blocks;
	uint32_t unallocated_blocks;
	uint32_t unallocated_inodes;
	uint32_t superblock_block;
	uint32_t block_size; 		// log2 (block size)
	uint32_t fragment_size;		// log2 (fragment size)
	uint32_t blocks_per_bgrp;
	uint32_t fragments_per_bgrp;
	uint32_t inodes_per_bgrp;
	uint32_t last_mount_time_posix;
	uint32_t last_written_time_posix;
	uint16_t mounted_since_consistency_check;
	uint16_t mounts_before_consistency_check;
	uint16_t ext2_signature;
	uint16_t fs_state;
	uint16_t proc_if_error;
	uint16_t version_minor;
	uint32_t consistency_check_posix;
	uint32_t interval_consistency_checks_posix;
	uint32_t os_id_created_fs;
	uint32_t version_major;
	uint16_t user_id_reserved_blocks;
	uint16_t grp_id_reserved_blocks;

} __attribute__ ((__packed__));

//extended superblock fields present if major version is >= to 1
struct extended_superblock {

        uint32_t first_non_reserved_inode;
	uint16_t inode_size;
	uint16_t bgrp_of_this_superblock;
	uint32_t optional_features;
	uint32_t required_features;
	uint32_t read_only_features_not_present;	//features that if not present, fs must be read-only
	uint8_t fs_id [16];
	uint8_t volume_name [16];
	uint8_t path_volume_last_mount [64];
	uint32_t compression_algo;
	uint8_t preallocate_blocks_file;
	uint8_t preallocate_blocks_dir;
	uint16_t reserved;				//unused two bytes
	uint8_t journal_id [16];
	uint32_t journal_inode;
	uint32_t journal_device;
	uint32_t orphan_inode_list_head;
	uint8_t unused [788];				//unused last bytes

} __attribute__ ((__packed__));

//Optional Feature Flags
#define OF_PREALLOCATE_BLOCKS 0x1				//preallocate contiguous blocks to directory when creating new one
#define OF_AFS_SERVER_INODES_EXIST 0x2
#define OF_FS_HAS_JOURNAL 0x4
#define OF_INODE_HAS_EXTENDED_ATTR 0x8
#define OF_FS_CAN_RESIZE 0x10					//fs can resize for larger partitions
#define OF_DIR_USE_HASH_INDEX 0x20

//Required Feature Flags
#define RF_COMPRESSION_USED 0x1
#define RF_DIR_ENTRIES_CONTAIN_TYPE 0x2
#define RF_FS_REPLAY_JOURNAL 0x4
#define RF_FS_USES_JOURNAL_DEVICE 0x8

//Read-Only Feature Flags
#define ROF_SPARSE_SBLOCK_GRP_DES_TABLE 0x1
#define ROF_FS_USES_64_BIT_FILE_SIZE 0x2
#define ROF_DIR_BINARY_TREE 0x4

struct 	block_group_descriptor_table {

	uint32_t block_addr_block_usage_map;
	uint32_t block_addr_inode_usage_map;
	uint32_t block_addr_inode_table;
	uint16_t unallocated_blocks_in_grp;
	uint16_t unallocated_inodes_in_grp;
	uint16_t dirs_in_bgrp;
	uint8_t unused [14];

} __attribute__ ((__packed__));

//Inode Type
#define I_FIFO 		0x1000
#define I_CHARACTER_DEVICE 0x2000
#define I_DIRECTORY 	0x4000
#define I_BLOCK_DEVICE 	0x6000
#define I_REGULAR_FILE 	0x8000
#define I_SYMBOLIC_LINK 0xA000
#define I_UNIX_SOCKET 	0xC000

//Inode Permissions
#define I_OTHER_EXECUTE 0x1
#define I_OTHER_WRITE 	0x2
#define I_OTHER_READ 	0x4
#define I_GROUP_EXECUTE 0x8
#define I_GROUP_WRITE 	0x10
#define I_GROUP_READ 	0x20
#define I_USER_EXECUTE 	0x40
#define I_USER_WRITE 	0x80
#define I_USER_READ 	0x100
#define I_STICKY 	0x200
#define I_SET_GROUP_ID 	0x400
#define I_SET_USER_ID 	0x800

//Inode Flags
#define I_SECURE_DELETION 	0x1
#define I_KEEP_COPY_DELETION 	0x2
#define I_FILE_COMPRESSION 	0x4
#define I_SYNC_UPDATES 		0x8
#define I_INMUTABLE_FILE 	0x10
#define I_APPEND_ONLY 		0x20
#define I_FILE_NOT_INC_DUMP 	0x40
#define I_LAST_ACCESS_NO_UDPATE 0x80
#define I_HASH_INDEXED_DIR 	0x10000
#define I_AFS_DIR 		0x20000
#define I_JOURNAL_FILE_DATA 	0x40000

//OS Specific Value 2

struct os_value_2_linux {
	uint8_t fragment_number;
	uint8_t fragment_size;
	uint16_t reserved_0;
	uint16_t user_id_high;
	uint16_t grp_id_high;
	uint32_t reserved_1;
} __attribute__ ((__packed__));

struct os_value_2_HURD {
	uint8_t fragment_number;
	uint8_t fragment_size;
	uint16_t type_permissions_high;
	uint16_t user_id_high;
	uint16_t grp_id_high;
	uint32_t user_id_author;		//if 0xffffffff normal user id used
} __attribute__ ((__packed__));

struct os_value_2_MASIX {
	uint8_t fragment_number;
	uint8_t fragment_size;
	uint8_t reserved [10];
} __attribute__ ((__packed__));

struct inode {

	uint16_t type_permissions;
	uint16_t user_id;
	uint32_t file_size_low;
	uint32_t last_access_posix;
	uint32_t creation_time_posix;
	uint32_t last_mod_time_posix;
	uint32_t deletion_time_posix;
	uint16_t group_id;
	uint16_t hard_links;
	uint32_t inode_disk_sectors;
	uint32_t flags;
	uint32_t os_value_1;
	uint32_t direct_block_pointer_0;
	uint32_t direct_block_pointer_1;
	uint32_t direct_block_pointer_2;
	uint32_t direct_block_pointer_3;
	uint32_t direct_block_pointer_4;
	uint32_t direct_block_pointer_5;
	uint32_t direct_block_pointer_6;
	uint32_t direct_block_pointer_7;
	uint32_t direct_block_pointer_8;
	uint32_t direct_block_pointer_9;
	uint32_t direct_block_pointer_10;
	uint32_t direct_block_pointer_11;
	uint32_t singly_indirect_block_pointer;
	uint32_t doubly_indirect_block_pointer;
	uint32_t triply_indirect_block_pointer;
	uint32_t generation_number;
	uint32_t extended_attribute_block;
	uint32_t file_size_high_dir_acl;		//upper 32 bits of file size (if feature bit set) if file, or dir ACL if dir
	uint32_t fragment_block_addr;
	uint8_t os_value_2 [12];

} __attribute__ ((__packed__));

//Directory Entry Type Indicators

#define DE_UNKNOWN 		0x0
#define DE_REGULAR_FILE 	0x1
#define DE_DIR 			0x2
#define DE_CHARACTER_DEVICE 	0x3
#define DE_BLOCK_DEVICE 	0x4
#define DE_FIFO 		0x5
#define DE_SOCKET 		0x6
#define DE_SYMBOLIC_LINK 	0x7			//soft link

struct dir_entry {

	uint32_t inode;
	uint16_t size;
	uint8_t name_length_low;
	uint8_t type_or_name_length_high;		//if feature bit set for "dir entries have file type byte" type, otherwise least significant bits of name length
	uint8_t name[0x100];

} __attribute__ ((__packed__));

extern struct dir_entry current_working_dir;

struct dir_entry filesystem_search (uint8_t * file);
void print_dir (uint32_t inode);
void change_dir (struct dir_entry _dir_entry, uint8_t * buffer);
void mkdir (uint8_t * address);
void rmdir (uint8_t * address);


#endif
