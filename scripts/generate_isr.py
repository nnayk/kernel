TEMPLATE = """\
global isr{}
isr{}:
    push rsi ; placeholder for error code
    push rsi ; will be used to store irq number
    mov rsi, {}
    jmp isr_glue
"""

NUM_ISRS = 256
for i in range(NUM_ISRS):
    data = TEMPLATE.format(i, i, i)
    print(data)
