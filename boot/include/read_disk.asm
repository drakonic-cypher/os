; the disk contains 1024 cylinders, 16 heads, and 63 sectors per track
; each sector is 512 bytes. Sectors passed to read_disk must be set
; starting at 0 so the 0th offset on disk is sector 0. But INT 0x13
; starts numbering sectors at 1, so sector number must be incremented
; before the interrupt.

; the problem is the sectors are written in the 0th head followed
; by the 1st head before the second cylinder is touched

%define SECTOR_SIZE 0x200
%define CYLINDERS 0x400
%define HEADS 0X10
%define SPT 0x3f
%define SPC 0x3f0	; 63 * 16 (sectors per cylinder)

read_disk:

jmp init_read_disk

RESET_MESSAGE: db 'Reset Error! ', 0
READ_MESSAGE: db 'Read Error! ', 0
;READ_SUCCESS: db 'Read Success! ', 0

init_read_disk:

push bp
mov bp, sp
pusha
push es

mov ah, 0x0 		;reset disk
;mov dl, 0x0
mov dl, 0x80 		;drive number (0x0 for floppy, 0x80 for disk 0)

int 0x13

jnc start_read_disk 	;carry flag set if there is an error

push RESET_MESSAGE
call print_string
pop ax
jmp end_read_disk

HEAD_NUMBER: 	db 0x0
TRACK_NUMBER: 	dw 0x0
SECTOR_NUMBER: 	db 0x0

start_read_disk:

mov dx, [bp + 8] ; move high bytes of sector value into dx
mov ax, [bp + 6] ; move low bytes of sector value into ax

mov cx, SPC
div cx		 ; ax is the track number, dx has the remainder for head and sectors

mov WORD [TRACK_NUMBER], ax

mov ax, dx
xor dx, dx
mov cx, SPT
div cx

inc dl		 ; sector number passed to INT 0x13 has to be (1-63)

mov BYTE [HEAD_NUMBER], al
mov BYTE [SECTOR_NUMBER], dl

and ax, 0x300 	 ; select 8-9 bits
shr ax, 0x2
mov cl, [SECTOR_NUMBER]
or cl, al	 ; cl now has sector number at first 0-5 bits and high bits of track number at 6-7 bits

mov WORD ax, [TRACK_NUMBER]
mov ch, al	 ; ch now contains first 8 bits of track number

mov ax, [bp + 4] ; if value is 0x7e0 this means 0x7e00 (to get physical address, segment address is multiplied by 16 => shifted 4 bits)
mov es, ax

mov ah, 0x2 ;read disk command
mov al, [bp + 10] ;number of sectors
mov dh, [HEAD_NUMBER] ;head number
;mov dl, 0x0
mov dl, 0x80 ;drive number
mov bx, 0x0 ;pointer to memory (offset by es segment)

int 0x13

jnc success_read_disk

push READ_MESSAGE
call print_string
pop ax
jmp end_read_disk

success_read_disk:

;push READ_SUCCESS
;call print_string
;pop ax

end_read_disk:

pop es
popa
mov sp, bp
pop bp

ret
