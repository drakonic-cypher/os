; takes length of known string, address to known string, and string to check
; returns true (0x1) or false (0x0) through ax register

compare_string:

push bp
mov bp, sp
pusha

xor ax, ax

mov cx, [bp + 8]	; use length as counter
mov si, [bp + 6]	; address of known string
mov di, [bp + 4]	; address of string to check
add si, cx
add di, cx

cmp BYTE [di], 0x0	; check null character of string to check, should be located at length
jne string_false
dec si
dec di

loop_compare_string:

mov bl, [di]
cmp [si], bl
jne string_false
dec cx
dec si
dec di
cmp cx, 0x0
jne loop_compare_string

string_true:

popa
mov ax, 0x1
jmp clean_up_compare_string

string_false:

popa
xor ax, ax

clean_up_compare_string:

mov sp, bp
pop bp

ret
