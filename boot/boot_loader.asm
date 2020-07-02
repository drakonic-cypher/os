ORG 0x7c00
BITS 16

jmp init

HELLO_MESSAGE: db 0xa, 'Loading Mystix... ', 0

%include "print_string.asm"
%include "compare_string.asm"
%include "read_disk.asm"
%include "get_file_name.asm"
%include "load_read_superblock.asm"

init:

xor ax, ax
mov ds, ax 	; set up the data and extra segments (ensure correct addressing)
mov es, ax

mov bp, 0xfffe ;prevent data overwrite of stack by read disk (stack cannot be set higher in 16 bit mode)
mov sp, bp

push HELLO_MESSAGE
call print_string
pop ax

push 0x1	; number of sectors to read
push 0x0	; high value of sector
push 0x1	; low value of sector to read from (passed numbers start at 0)
push 0x7e0 	; pass address to read to
call read_disk
pop ax
pop ax
pop ax
pop ax

call load_read_superblock

jmp init2	; 0x7e00

times 510 - ($-$$) db 0
dw 0xaa55

boot_loader_2:

FILE_NOT_FOUND_MSG: db 0xd, 0xa, 0xa,' File Not Found!', 0

%include "find_file.asm"

init2:

call get_file_name

push ax
call find_file
pop dx

cmp ax, 0x1
je load_kernel

push FILE_NOT_FOUND_MSG
call print_string
pop ax

jmp init2

load_kernel:

jmp 0x500

times 512 - ($-boot_loader_2) db 0

