global idt_start

section .bss
    ;temp resb 4096
    idt resb 4096
section .text
bits 64
idt_start:
    mov rax, 0x2f592f412f4b2f4f
    mov qword [0xb8000], rax
    cli
idt_descriptor:
    mov word [idt_descriptor], 4095
    mov qword [idt_descriptor+2], idt
mov rax,idt_descriptor
lidt [rax]
ret
    
