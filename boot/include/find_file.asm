; scan boot directory in root directory to find kernel file input by user

; read superblock, find size of blocks, num blocks per group, number of
; inodes per group, and starting block of first group (blk grp descriptor tab)
; determine block group of inode
; read block group descriptor corresponding to group that contains inode
; from block group descriptor extract location of block group inode table
; determine index of inode in inode table
; index the inode table

; the root dir (/) is inode 2 always
; the block descriptor table is located in the block immediately after superblock
; it contains descriptors of 32 bytes for all block groups in file system
; the block address of the location of the inode table is in byte 8 of each entry.
; therefore, from block size, get sector number (ie. if block size is 1024 and
; each sector is 512, then the block number is multiplied by 2 to get sector).

; the blocks will be loaded to address 0x500 (and the needed information will
; be buffered, this area will be overwritten by the kernel when loaded

; block group = (inode - 1) / INODES_PER_BGROUP
; index inside inode table = (inode - 1) % INODES_PER_BGROUP

ROOT_INODE_TABLE: dd 0x0	; block address of root dir inode table (byte 8 - BGD table)
KERNEL_INODE_TABLE: dd 0x0	; block address of kenel file inode table

ROOT_INODE_INDEX: dw 0x1
KERNEL_INODE_INDEX: dw 0x0

ROOT_GRP_DES_OFFSET: dw 0x0	; offset into group descriptor table
KERNEL_GRP_DES_OFFSET: dw 0x0

ROOT_INODE: dd 0x2		; root dir inode
KERNEL_INODE: dd 0x0		; inode number of kernel file

ROOT_DIR_BLOCK_ADDR: dd 0x0	; block address of root directory data

FILE_NAME_ADDR: dw 0x0		; address of file name input by user

%define BASE_ADDR 0x500		; address disk reads to mainly
%define INODE_BASE_ADDR 0x8000  ; inode for kernel file needs to be kept in memory
%define KERNEL_START 0x50

%define GRP_DES_ADDR 0x900	; only the grp des table will be in another address
%define INODE_SIZE 0x80 	; maybe just use this as fixed
%define GRP_DES_SIZE 0x20	; use this to multiply block group by
%define SECTOR_SIZE 0x200

%define INDIRECT_BLOCK_ADDR 0x3500

setup_indirect_read:

mov di, 0x8000
mov si, 0x3500
mov cx, 0x100
rep movsd

mov si, 0x8000
mov di, 0x350
mov cx, 0xd

jmp loop_read_kernel_inode


; this is the final function that reads the kernel file data to address 0x500
read_kernel_inode:

pusha

xor dx, dx
mov ax, INODE_SIZE
mov WORD cx, [KERNEL_INODE_INDEX]

mul cx

mov si, INODE_BASE_ADDR + 0x28  		; the 40th byte into the inode entry is the first data block
add si, ax

mov di, KERNEL_START
xor cx, cx
xor bx, bx
mov bl, [SECTORS_PER_BLOCK]

loop_read_kernel_inode:

mov WORD dx, [si + 2]
mov WORD ax, [si]
or dx, ax
cmp dx, 0x0
je clean_up_read_kernel_inode	; there are no more data blocks

mov WORD dx, [si + 2]
mul bx

push bx				; read number of sectors per block
push dx
push ax
push di
call read_disk
pop di
pop ax
pop ax
pop ax

cmp cx, 0xc
je setup_indirect_read		; at this point all first 12 blocks have been read, indirect block at 0x3500, copy to 0x8000

mov ax, 0x4
add si, ax
add di, 0x40			; add a block size to di
inc cx
jmp loop_read_kernel_inode

clean_up_read_kernel_inode:

popa

ret

read_root_dir_entries:

pusha

xor cx, cx
mov cl, [SECTORS_PER_BLOCK]

mov dx, [ROOT_DIR_BLOCK_ADDR + 2]
mov ax, [ROOT_DIR_BLOCK_ADDR]

mul cx

push 0x2			; read a block
push dx
push ax
push 0x50
call read_disk
pop ax
pop ax
pop ax
pop ax

xor cx, cx			; this will hold total size of data read
mov si, BASE_ADDR

loop_read_root_dir:

cmp cx, 0x400			; size of a block
je file_not_found		; all entries already read

xor ax, ax
mov al, [si + 6]		; size of name
push ax
lea bx, [si + 8]		; pointer to name of file
push bx
push WORD [FILE_NAME_ADDR]
call compare_string
pop bx
pop bx
pop bx

cmp ax, 0x1			; check if function returned true
je file_found

mov ax, [si + 4]		; this is the size of the dir entry
add si, ax			; move si to the next entry
add cx, ax			; keeps count of total bytes read
jmp loop_read_root_dir

file_not_found:
popa
mov ax, 0x0
jmp clean_up_read_root_dir

file_found:
mov di, KERNEL_INODE		; si is pointing to beginning of the directory entry (inode number)
mov cx, 0x1
movsd

popa
mov ax, 0x1
clean_up_read_root_dir:

ret

read_inode_table:

push bp
mov bp, sp
pusha

xor dx, dx
mov ax, INODE_SIZE
mov bx, [bp + 4]
mov cx, [bx]                    ; Inode index

mul cx

mov si, INODE_BASE_ADDR + 0x28  ; byte 40 is the starting address of data contents
add si, ax

mov di, [bp + 6]                ; buffer to store data address
mov cx, 0x1
movsd

popa
mov sp, bp
pop bp

ret

load_inode_table:

push bp
mov bp, sp
pusha

xor cx, cx
mov cl, [SECTORS_PER_BLOCK]

mov si, [bp + 4]		; address of buffer holding block number of inode table

mov WORD dx, [si + 2]
mov WORD ax, [si]

mul cx				; dx:ax is block number, mul by sectors per block

push 0xa			; need to load enough sectors to read complete inode table
push dx
push ax
push 0x800			; the inode table will be read at 0x8000, right after the two boot sectors
call read_disk
pop ax
pop ax
pop ax
pop ax

popa
mov sp, bp
pop bp

ret

; push inode number and buffers on stack before call
calc_bgrp_inode_index:

push bp
mov bp, sp

mov si, [bp + 4]	; get buffer address from stack
mov dx, [si + 2]	; high bits of inode
mov ax, [si]		; low bits of inode
sub ax, 0x1		; inode number must be 1 lower

jnc continue_grp_des_calc
dec dx

continue_grp_des_calc:

mov cx, [INODES_PER_BGROUP]
div cx				; use lower bits

mov di, [bp + 6]
mov [di], dx			; this will be the inode index

mov cx, GRP_DES_SIZE
mul cx				; multiply the block group by grp_des size

mov di, [bp + 8]
mov [di], ax 			; store offset into buffer passed before call

mov sp, bp
pop bp

ret

; block group descriptor table contains descriptors for every block group in file system, always located after superblock
read_grp_des_table:

push bp
mov bp, sp
pusha

; use group number to offset
mov bx, [bp + 6]
mov ax, [bx]

mov si, GRP_DES_ADDR + 0x8
add si, ax
mov di, [bp + 4]
mov cx, 0x1
movsd

popa
mov sp, bp
pop bp

ret

find_file:

push bp
mov bp, sp
pusha

mov ax, [bp + 4]
mov [FILE_NAME_ADDR], ax		; get address of file name from stack

push ROOT_GRP_DES_OFFSET
push ROOT_INODE_TABLE		; the buffer values for root inode are already determined
call read_grp_des_table
pop ax
pop ax

push ROOT_INODE_TABLE
call load_inode_table
pop ax

push ROOT_DIR_BLOCK_ADDR
push ROOT_INODE_INDEX
call read_inode_table
pop ax
pop ax

call read_root_dir_entries
cmp ax, 0x0
je find_file_failed		; file was not found in root directory, exit find file

push KERNEL_GRP_DES_OFFSET
push KERNEL_INODE_INDEX
push KERNEL_INODE
call calc_bgrp_inode_index
pop ax
pop ax
pop ax

push KERNEL_GRP_DES_OFFSET
push KERNEL_INODE_TABLE
call read_grp_des_table
pop ax
pop ax

push KERNEL_INODE_TABLE
call load_inode_table
pop ax

call read_kernel_inode

popa
mov ax, 0x1			; return success in ax
jmp clean_up_find_file

find_file_failed:

popa
xor ax, ax			; return fail code in ax

clean_up_find_file:

mov sp, bp
pop bp

ret
