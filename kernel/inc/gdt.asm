[GLOBAL gdt_flush]
[extern gdtr_pointer]

gdt_table:

;ALL DESCRIPTORS ARE 8 BYTES IN SIZE

;FIRST DESCRIPTOR, NULL DESCRIPTOR
dd 0x0
dd 0x0

;KERNEL. SECOND DESCRIPTOR, CODE DESCRIPTOR

dw 0xffff  	;low limit
dw 0x0     	;base low
db 0x0     	;base middle
db 10011010b    ;access
db 11001111b    ;granularity
db 0x0     	;base high

;KERNEL. THIRD DESCRIPTOR, DATA DESCRIPTOR

dw 0xffff
dw 0x0
db 0x0
db 10010010b
db 11001111b
db 0x0

;USER. FOURTH DESCRIPTOR, CODE DESCRIPTOR

dw 0xffff
dw 0x0
db 0x0
db 11111010b
db 11001111b
db 0x0

;USER. FIFTH DESCRIPTOR, DATA DESCRIPTOR

dw 0xffff
dw 0x0
db 0x0
db 11110010b
db 11001111b
db 0x0

gdt_table_end:

gdt_table_pointer:
dw gdt_table_end - gdt_table - 1
dd gdt_table
