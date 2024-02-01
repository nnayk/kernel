global long_mode_start
extern kmain
extern idt_descriptor

section .text
bits 64
long_mode_start:
    ; load 0 into all data segment registers
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ; print `OKAY` to screen
    ;mov rax, 0x2f592f412f4b2f4f
    ;mov qword [0xb8000], rax
    ;call idt_descriptor 
    call kmain
    hlt
