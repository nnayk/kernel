TEMPLATE = """
global isr{}
isr{}:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, {}
    jmp isr_glue
"""

NUM_ISRS = 256
for i in range(NUM_ISRS):
    data = TEMPLATE.format(i, i, i)
    print(data)
