global idt_descriptor
section .bss
    idt resb 4096
section .text
bits 64
idt_descriptor:
    mov word [idt_descriptor], 4095
    mov qword [idt_descriptor+2], idt
    mov rax,idt_descriptor
    lidt [rax]
    ret
    
