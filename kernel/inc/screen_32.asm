%define VIDMEM 0xb8000
%define TEXT_COLOR 0x1f

%define SCREEN_WIDTH 0x50
%define SCREEN_HEIGHT 0x19

%define CRT_INDEX_REGISTER 0x3d4
%define CRT_DATA_REGISTER 0x3d5
%define CURSOR_HIGH_BYTE_INDEX 0xe
%define CURSOR_LOW_BYTE_INDEX 0xf
%define CURSOR_X_OFFSET 0x0
%define CURSOR_Y_OFFSET 0x320	;line number * SCREEN WIDTH

print_string_32:

push ebp
mov ebp, esp
pushad

mov esi, [ebp + 8] 		;get address of string passed onto stack before call
mov edi, VIDMEM
mov eax, CURSOR_Y_OFFSET
imul eax, 0x2
add edi, eax
mov eax, CURSOR_X_OFFSET
imul eax, 0x2
add edi, eax
mov ebx, CURSOR_Y_OFFSET 	;cursor position
add ebx, CURSOR_X_OFFSET

loop_print_string_32:

; move the cursor

mov al, CURSOR_HIGH_BYTE_INDEX
mov dx, CRT_INDEX_REGISTER
out dx, al

mov al, bh
mov dx, CRT_DATA_REGISTER
out dx, al

mov al, CURSOR_LOW_BYTE_INDEX
mov dx, CRT_INDEX_REGISTER
out dx, al

mov al, bl
mov dx, CRT_DATA_REGISTER
out dx, al

mov al, [esi]
cmp al, 0x0
jz clean_up_print_string_32

mov BYTE [edi], al

; continue loop

inc ebx
inc esi
add edi, 0x2
jmp loop_print_string_32

clean_up_print_string_32:

popad
mov esp, ebp
pop ebp

ret

clear_screen:

push ebp
mov ebp, esp
pushad

xor ecx, ecx
mov eax, SCREEN_WIDTH
mov edx, SCREEN_HEIGHT
imul eax, edx
mov edx, 0x2 ;two bytes per character on screen
imul eax, edx

loop_clear_screen:

mov BYTE [VIDMEM + ecx], 0x0 ;output nothing
mov BYTE [VIDMEM + ecx + 1], TEXT_COLOR
add ecx, 0x2
cmp ecx, eax
jne loop_clear_screen

call draw_frame

popad
mov esp, ebp
pop ebp

ret

%define FRAME_CHAR_1 0xba
%define FRAME_CHAR_2 0xcd
%define FRAME_CHAR_3 0xc9
%define FRAME_CHAR_4 0xbb
%define FRAME_CHAR_5 0xc8
%define FRAME_CHAR_6 0xbc

%define FRAME_X_OFFSET 0x6
%define FRAME_Y_OFFSET 0x0
%define FRAME_HEIGHT 0x8
%define FRAME_WIDTH 0x90	;72 pixels * 2 (2 bytes per pixel)

draw_frame:

push ebp
mov ebp, esp
pushad

mov edi, VIDMEM
mov eax, FRAME_Y_OFFSET
add edi, eax
mov eax, FRAME_X_OFFSET
add edi, eax
mov ecx, FRAME_WIDTH

mov BYTE [edi], FRAME_CHAR_3		;first character is a corner
add edi, 0x2

loop_draw_frame_1:

mov BYTE [edi], FRAME_CHAR_2
add edi, 0x2
sub ecx, 0x2
cmp ecx, 0x0
jne loop_draw_frame_1

mov BYTE [edi], FRAME_CHAR_4

add edi, 0x500				; frame height * window width

mov BYTE [edi], FRAME_CHAR_6
sub edi, 0x2

loop_draw_frame_2:

mov BYTE [edi], FRAME_CHAR_2
sub edi, 0x2
add ecx, 0x2
cmp ecx, FRAME_WIDTH
jne loop_draw_frame_2

mov BYTE [edi], FRAME_CHAR_5

loop_draw_frame_3:

clean_up_draw_frame:

popad
mov esp, ebp
pop ebp

ret

LINE_1: db 0x00, 0x02, 0x06, 0x08, 0x0e, 0x16, 0x1e, 0x20, 0x22, 0x28, 0x2a, 0x2c, 0x32, 0x34, 0x36, 0x3c, 0x42
LINE_2: db 0x00, 0x04, 0x08, 0x10, 0x14, 0x1c, 0x1e, 0x2a, 0x34, 0x3e, 0x40
LINE_3: db 0x00, 0x04, 0x08, 0x12, 0x20, 0x22, 0x2a, 0x34, 0x3e, 0x40
LINE_4: db 0x00, 0x08, 0x12, 0x1c, 0x1e, 0x20, 0x2a, 0x32, 0x34, 0x36, 0x3c, 0x42


SUBTITLE: db 'A RETRO OPERATING SYSTEM', 0

%define LINE_1_LENGTH 0x11
%define LINE_2_LENGTH 0xb
%define LINE_3_LENGTH 0xa
%define LINE_4_LENGTH 0xc

%define SPLASH_X_OFFSET 0x2e
%define SPLASH_Y_OFFSET 0xa0	; top line number * WINDOW_WIDTH * 2 (bytes per pixel)
%define FILL_CHAR 0xb2

%define SUBTITLE_OFFSET 0x498

draw_splash_line:

push ebp
mov ebp, esp
pushad

mov edi, VIDMEM
mov eax, SPLASH_X_OFFSET
mov ebx, DWORD [ebp + 8]	; get address of line bit map
mov ecx, DWORD [ebp + 12] 	; get length of line bit map (will act as counter)
mov edx, DWORD [ebp + 16] 	; get y-offset

add edi, edx
add edi, eax
mov esi, edi	; make copy of calculation

loop_splash_line:

cmp ecx, 0x0
je clean_up_splash_line

dec ecx
xor eax, eax
mov al, [ebx + ecx]
add edi, eax
mov BYTE [edi], FILL_CHAR

mov edi, esi	; reset from copy
jmp loop_splash_line

clean_up_splash_line:

popad
mov esp, ebp
pop ebp

ret

mystix_splash:

push ebp
mov ebp, esp
pushad

draw_line_1:

push DWORD SPLASH_Y_OFFSET
push DWORD LINE_1_LENGTH
push LINE_1
call draw_splash_line
pop eax
pop eax
pop eax

draw_line_2:

mov eax, SPLASH_Y_OFFSET
add eax, 0xa0

push eax
push DWORD LINE_2_LENGTH
push LINE_2
call draw_splash_line
pop eax
pop eax
pop eax

draw_line_3:

mov eax, SPLASH_Y_OFFSET
add eax, 0x140

push eax
push DWORD LINE_3_LENGTH
push LINE_3
call draw_splash_line
pop eax
pop eax
pop eax

draw_line_4:

mov eax, SPLASH_Y_OFFSET
add eax, 0x1e0

push eax
push DWORD LINE_4_LENGTH
push LINE_4
call draw_splash_line
pop eax
pop eax
pop eax

print_sub_title:

mov edi, VIDMEM
add edi, SUBTITLE_OFFSET
mov esi, SUBTITLE
xor ecx, ecx

loop_print_sub_title:

mov al, [esi + ecx]
cmp al, 0x0
je clean_up_splash

mov [edi], al

inc ecx
add edi, 0x2
jmp loop_print_sub_title

clean_up_splash:

popad
mov esp, ebp
pop ebp

ret
