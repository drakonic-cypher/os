[GLOBAL _isr0]
[GLOBAL _isr1]
[GLOBAL _isr2]
[GLOBAL _isr3]
[GLOBAL _isr4]
[GLOBAL _isr5]
[GLOBAL _isr6]
[GLOBAL _isr7]
[GLOBAL _isr8]
[GLOBAL _isr9]
[GLOBAL _isr10]
[GLOBAL _isr11]
[GLOBAL _isr12]
[GLOBAL _isr13]
[GLOBAL _isr14]
[GLOBAL _isr15]
[GLOBAL _isr16]
[GLOBAL _isr17]
[GLOBAL _isr18]
[GLOBAL _isr19]
[GLOBAL _isr20]
[GLOBAL _isr21]
[GLOBAL _isr22]
[GLOBAL _isr23]
[GLOBAL _isr24]
[GLOBAL _isr25]
[GLOBAL _isr26]
[GLOBAL _isr27]
[GLOBAL _isr28]
[GLOBAL _isr29]
[GLOBAL _isr30]
[GLOBAL _isr31]

[GLOBAL _irq0]
[GLOBAL _irq1]
[GLOBAL _irq2]
[GLOBAL _irq3]
[GLOBAL _irq4]
[GLOBAL _irq5]
[GLOBAL _irq6]
[GLOBAL _irq7]
[GLOBAL _irq8]
[GLOBAL _irq9]
[GLOBAL _irq10]
[GLOBAL _irq11]
[GLOBAL _irq12]
[GLOBAL _irq13]
[GLOBAL _irq14]
[GLOBAL _irq15]


;  0: Divide By Zero Exception
_isr0:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep a
                   ; uniform stack frame
    push byte 0
    jmp isr_common_stub

;  1: Debug Exception
_isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

_isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

_isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

_isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

_isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

_isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

;isr8 pushes an error code, so should not push another byte
_isr8:
    cli
    push byte 8
    jmp isr_common_stub

_isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

_isr10:
    cli
    push byte 10
    jmp isr_common_stub

_isr11:
    cli
    push byte 11
    jmp isr_common_stub

_isr12:
    cli
    push byte 12
    jmp isr_common_stub

_isr13:
    cli
    push byte 13
    jmp isr_common_stub

_isr14:
    cli
    push byte 14
    jmp isr_common_stub

_isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

_isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

_isr17:
    cli
    push byte 17
    jmp isr_common_stub

_isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

_isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

_isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

_isr21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

_isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

_isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

_isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

_isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

_isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

_isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

_isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

_isr29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

_isr30:
    cli
    push byte 30
    jmp isr_common_stub

_isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub

; We call a C function in here. We need to let the assembler know
; that '_fault_handler' exists in another file

extern fault_handler

; This is the common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   	; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   	; Push us the stack
    push eax
    mov eax, fault_handler
    call eax       	; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8     	; Cleans up the pushed error code and pushed ISR number
    iretd           	; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!



; 32: IRQ0
_irq0:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 32
    jmp irq_common_stub

_irq1:
    cli
    push byte 0
    push byte 33
    jmp irq_common_stub

_irq2:
    cli
    push byte 0
    push byte 34
    jmp irq_common_stub

_irq3:
    cli
    push byte 0
    push byte 35
    jmp irq_common_stub

_irq4:
    cli
    push byte 0
    push byte 36
    jmp irq_common_stub

_irq5:
    cli
    push byte 0
    push byte 37
    jmp irq_common_stub

_irq6:
    cli
    push byte 0
    push byte 38
    jmp irq_common_stub

_irq7:
    cli
    push byte 0
    push byte 39
    jmp irq_common_stub

_irq8:
    cli
    push byte 0
    push byte 40
    jmp irq_common_stub

_irq9:
    cli
    push byte 0
    push byte 41
    jmp irq_common_stub

_irq10:
    cli
    push byte 0
    push byte 42
    jmp irq_common_stub

_irq11:
    cli
    push byte 0
    push byte 43
    jmp irq_common_stub

_irq12:
    cli
    push byte 0
    push byte 44
    jmp irq_common_stub

_irq13:
    cli
    push byte 0
    push byte 45
    jmp irq_common_stub

_irq14:
    cli
    push byte 0
    push byte 46
    jmp irq_common_stub

; 47: IRQ15
_irq15:
    cli
    push byte 0
    push byte 47
    jmp irq_common_stub

[extern irq_handler]

; This is a stub that we have created for IRQ based ISRs. This calls
; '_irq_handler' in our C code. We need to create this in an 'irq.c'

irq_common_stub:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, irq_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8
    iretd
