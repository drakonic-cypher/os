STRING_BUFFER: db '                    ', 0

COMMAND_PROMPT: db 0xd, 0xa, 0xa, 'mystix:>', 0

get_file_name:

push bp
mov bp, sp
pusha

push COMMAND_PROMPT
call print_string
pop ax

mov bx, STRING_BUFFER
mov cx, 0x14		; read up to 20 bytes

loop_get_file_name:

mov ah, 0x0		; get char with INT 16

int 0x16

cmp al, 0x8		; backspace
jne check_carriage_get_file_name

cmp cx, 0x14		; so that cursor does not move past prompt
je loop_get_file_name

mov ah, 0xe

int 0x10

mov al, 0x20
mov ah, 0xe		; write character without moving cursor

int 0x10

mov al, 0x8

int 0x10

dec bx
inc cx			; increment count to allow for mistakes
mov BYTE [bx], 0x20	; replace with space
jmp loop_get_file_name

check_carriage_get_file_name:

cmp al, 0xd
je clean_up_get_file_name

continue_get_file_name:

mov ah, 0xe

int 0x10	; write it to screen

mov [bx], al
inc bx
dec cx
cmp cx, 0x0
je clean_up_get_file_name

jmp loop_get_file_name

clean_up_get_file_name:
mov BYTE [bx], 0x0	; null terminate string

popa
mov ax, STRING_BUFFER   ; return the address of the string
mov sp, bp
pop bp

ret
