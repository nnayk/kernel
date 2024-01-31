section .text
bits 64

global isr0
isr0:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 0
    call dummy
    pop rdi
    pop rsi
    iretq
global isr1
isr1:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 1
    call dummy
    pop rdi
    pop rsi
    iretq

global dummy
dummy:
    mov rax, 0x2f412f412f4b2f4f
    mov qword [0xb8050], rax
    ret