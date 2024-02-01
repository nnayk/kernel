;global idt_descriptor
global idt
section .bss
idt:
    resb 4096
;section .text
;bits 64
;idt_descriptor:
   ;mov word [idt_descriptor], 4095
   ;mov qword [idt_descriptor+2], idt2
   ;mov rax,idt_descriptor
   ;lidt [rax]
   ;ret
 ;  dw 4095
 ;  hlt
 ;  dq idt
 ;  hlt
;lidt [idt_descriptor]
;ret
