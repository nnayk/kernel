global idt_start

section .text
bits 64
idt_start:
    mov rax, 0x2f592f412f4b2f4f
    mov qword [0xb8000], rax
    cli
    ret
