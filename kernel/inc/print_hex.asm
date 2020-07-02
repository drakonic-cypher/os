;prints 16 bit hex number passed onto stack as '0x????'

PRINT_HEX_BUFFER: db '0x    ', 0

print_hex:

push bp
mov bp, sp
pusha

start_print_hex:

xor cx, cx 		; counter

loop_print_hex:

mov ax, [bp + 4] 	; copy number to manipulate
mov si, cx 		; copy counter to manipulate

shift_loop_print_hex:

cmp si, 0x0
je continue_print_hex
shr ax, 0x4
dec si
jmp shift_loop_print_hex

continue_print_hex:

and ax, 0xf
cmp ax, 0x9
jg print_hex_letter

mov si, 0x30 		; to print numbers need to add 0x30 but to print letters need to add 0x57 due to scan code
jmp offset_print_hex

print_hex_letter:

mov si, 0x57

offset_print_hex:

xor dx, dx
add ax, si
mov bx, 0x5 		; this will be offset address from print_hex_buffer
sub bx, cx
mov [PRINT_HEX_BUFFER + bx], al
inc cx
cmp cx, 0x4
jne loop_print_hex

finish_print_hex:

push PRINT_HEX_BUFFER
call print_string
pop ax

popa
mov sp, bp
pop bp

ret
