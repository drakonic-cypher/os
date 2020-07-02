; wait input has to be called before sending any command to the keyboard
; controller to make sure the buffer has been read before the next command.
; wait output is called until there is output to read 

set_a20_line:

pushad

call	wait_input
mov	al, 0xad
out	0x64, al	;disable keyboard
call 	wait_input

in	al, 0x60	; clear controller
call	wait_input

mov	al, 0xd0
out	0x64, al	;tell controller to read output port (returns output port data)
call	wait_output

in	al, 0x60
push	eax		;get output port data and store it
call 	wait_input

mov	al, 0xd1
out	0x64, al	;tell controller to write output port
call	wait_input

pop	eax
or	al, 0x2		;set bit 1 (this enables A20)
out	0x60, al	;write data back to the output port

call	wait_input
mov	al, 0xae	; re-enable keyboard
out 	0x64, al

call	wait_input
mov	al, 0xdf
out	0x64, al	;another way to enable a20 line just in case

call 	wait_input
in 	al, 0x92
or	al, 0x2
out 	0x92, al	;yet another way, fast-A20 line

popad

ret

;check until input read status of keyboard controller returns not busy
wait_input:
in 	al, 0x64
test 	al, 0x2
jnz	wait_input
ret

;check until there is output to read
wait_output:
in	al, 0x64
test	al, 0x1
jz	wait_output
ret
