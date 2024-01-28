TEMPLATE = '''extern void isr{}(void);'''

for i in range(256):
    print(TEMPLATE.format(i))
