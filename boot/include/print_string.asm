print_string:

push bp
mov bp, sp
pusha

mov bx, [bp + 4] ;get the address of the first character of the string passed to the function before call
xor dl, dl

loop_print_string:

cmp dl, [bx] ;only the bx register can be used for indexing
je clean_up_print_string

mov ah, 0xe
mov BYTE al, [bx]

int 0x10

inc bx
jmp loop_print_string

clean_up_print_string:

popa
mov sp, bp
pop bp

ret
