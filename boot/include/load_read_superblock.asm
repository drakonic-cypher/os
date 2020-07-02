BLOCK_SIZE: dd 0x0              ; this is the number of times 1024 is shifted left (byte 24)
BLOCKS_PER_BGROUP: dd 0x0       ; blocks per block group (byte 32)
INODES_PER_BGROUP: dd 0x0       ; to determine where inode is located (byte 40)
SECTORS_PER_BLOCK: db 0x0

%define BASE_ADDR 0x500
%define SECTOR_SIZE 0x200

load_read_superblock:

pusha

push 0x6        ; sectors to read (to get group des as well)
push 0x0        ; high value of sector
push 0x2        ; low value of sector to read from
push 0x50       ; address to read to
call read_disk
pop ax
pop ax
pop ax
pop ax

mov si, BASE_ADDR + 0x18                ; add 24 byte offset
mov di, BLOCK_SIZE
mov cx, 0x1
movsd

mov ax, 0x400                           ; shift 1024 by block size
xor dx, dx
mov cl, [BLOCK_SIZE]
shl ax, cl
mov cx, SECTOR_SIZE
div cx                                  ; ax contains sectors per block
mov [SECTORS_PER_BLOCK], al

mov si, BASE_ADDR + 0x20                ; add 32 byte offset
mov di, BLOCKS_PER_BGROUP
mov cx, 0x1
movsd

mov si, BASE_ADDR + 0x28                ; add 40 byte offset
mov di, INODES_PER_BGROUP
mov cx, 0x1
movsd

popa

ret

