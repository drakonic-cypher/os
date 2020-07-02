;START OF THE SECOND-STAGE BOOT LOADER

ORG 0x7e00
BITS 16

second_stage:

jmp init2

%include "find_file.asm"

%include "gdt.asm"

init2:

call find_file

cli

lgdt [gdt_table_pointer]

mov eax, cr0
or eax, 0x1
mov cr0, eax

;in order to set CS to use the gdt descriptor, need to far jump

jmp 0x8:transition_into_32

BITS 32

%include "set_a20_line.asm"

transition_into_32:
;need to set the DS, SS, ES, FS, GS segments to use the data descriptor in the gdt

mov ax, 0x10
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax
mov esp, 0x90000

;set the A-20 line using the most portable method (keyboard microcontroller)

call set_a20_line

jmp 0x500

times 512 - ($-second_stage) db 0
