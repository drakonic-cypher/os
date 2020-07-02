[extern kernel_main]

BITS 16

start:

jmp second_stage

%include "gdt.asm"

second_stage:

cli

lgdt [gdt_table_pointer]

mov eax, cr0
or eax, 0x1
mov cr0, eax

;in order to set CS to the gdt descriptor, need to far jump

jmp 0x8:transition_into_32

BITS 32

%include "set_a20_line.asm"
%include "screen_32.asm"
%include "idt.asm"

SETUP_COMMAND_PROMPT: db 0x0

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

third_stage:

call clear_screen

call mystix_splash

push SETUP_COMMAND_PROMPT
call print_string_32
pop eax

kernel_entry:

push 'R'
call kernel_main

jmp $

[extern gdtr_pointer]

gdt_flush:

lgdt [gdtr_pointer]
mov ax, 0x10
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

jmp 0x8:flush

flush:

ret

