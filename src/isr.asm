section .text
bits 64

global isr_glue
isr_glue:
    push rax
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11
    ;;call c_wrapper
    pop rax
    pop rcx
    pop rdx
    pop r8
    pop r9
    pop r10
    pop r11
    pop rdi 
    pop rsi ;; pop error code
    ret

global isr0
isr0:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 0
    jmp isr_glue

global isr1
isr1:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 1
    jmp isr_glue

;%if 0
global isr2
isr2:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 2
    jmp isr_glue


global isr3
isr3:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 3
    jmp isr_glue


global isr4
isr4:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 4
    jmp isr_glue


global isr5
isr5:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 5
    jmp isr_glue


global isr6
isr6:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 6
    jmp isr_glue


global isr7
isr7:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 7
    jmp isr_glue


global isr8
isr8:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 8
    jmp isr_glue


global isr9
isr9:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 9
    jmp isr_glue


global isr10
isr10:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 10
    jmp isr_glue


global isr11
isr11:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 11
    jmp isr_glue


global isr12
isr12:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 12
    jmp isr_glue


global isr13
isr13:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 13
    jmp isr_glue


global isr14
isr14:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 14
    jmp isr_glue


global isr15
isr15:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 15
    jmp isr_glue


global isr16
isr16:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 16
    jmp isr_glue


global isr17
isr17:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 17
    jmp isr_glue


global isr18
isr18:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 18
    jmp isr_glue


global isr19
isr19:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 19
    jmp isr_glue


global isr20
isr20:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 20
    jmp isr_glue


global isr21
isr21:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 21
    jmp isr_glue


global isr22
isr22:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 22
    jmp isr_glue


global isr23
isr23:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 23
    jmp isr_glue


global isr24
isr24:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 24
    jmp isr_glue


global isr25
isr25:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 25
    jmp isr_glue


global isr26
isr26:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 26
    jmp isr_glue


global isr27
isr27:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 27
    jmp isr_glue


global isr28
isr28:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 28
    jmp isr_glue


global isr29
isr29:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 29
    jmp isr_glue


global isr30
isr30:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 30
    jmp isr_glue


global isr31
isr31:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 31
    jmp isr_glue


global isr32
isr32:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 32
    jmp isr_glue


global isr33
isr33:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 33
    jmp isr_glue


global isr34
isr34:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 34
    jmp isr_glue


global isr35
isr35:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 35
    jmp isr_glue


global isr36
isr36:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 36
    jmp isr_glue


global isr37
isr37:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 37
    jmp isr_glue


global isr38
isr38:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 38
    jmp isr_glue


global isr39
isr39:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 39
    jmp isr_glue


global isr40
isr40:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 40
    jmp isr_glue


global isr41
isr41:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 41
    jmp isr_glue


global isr42
isr42:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 42
    jmp isr_glue


global isr43
isr43:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 43
    jmp isr_glue


global isr44
isr44:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 44
    jmp isr_glue


global isr45
isr45:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 45
    jmp isr_glue


global isr46
isr46:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 46
    jmp isr_glue


global isr47
isr47:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 47
    jmp isr_glue


global isr48
isr48:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 48
    jmp isr_glue


global isr49
isr49:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 49
    jmp isr_glue


global isr50
isr50:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 50
    jmp isr_glue


global isr51
isr51:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 51
    jmp isr_glue


global isr52
isr52:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 52
    jmp isr_glue


global isr53
isr53:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 53
    jmp isr_glue


global isr54
isr54:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 54
    jmp isr_glue


global isr55
isr55:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 55
    jmp isr_glue


global isr56
isr56:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 56
    jmp isr_glue


global isr57
isr57:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 57
    jmp isr_glue


global isr58
isr58:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 58
    jmp isr_glue


global isr59
isr59:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 59
    jmp isr_glue


global isr60
isr60:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 60
    jmp isr_glue


global isr61
isr61:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 61
    jmp isr_glue


global isr62
isr62:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 62
    jmp isr_glue


global isr63
isr63:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 63
    jmp isr_glue


global isr64
isr64:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 64
    jmp isr_glue


global isr65
isr65:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 65
    jmp isr_glue


global isr66
isr66:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 66
    jmp isr_glue


global isr67
isr67:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 67
    jmp isr_glue


global isr68
isr68:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 68
    jmp isr_glue


global isr69
isr69:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 69
    jmp isr_glue


global isr70
isr70:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 70
    jmp isr_glue


global isr71
isr71:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 71
    jmp isr_glue


global isr72
isr72:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 72
    jmp isr_glue


global isr73
isr73:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 73
    jmp isr_glue


global isr74
isr74:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 74
    jmp isr_glue


global isr75
isr75:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 75
    jmp isr_glue


global isr76
isr76:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 76
    jmp isr_glue


global isr77
isr77:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 77
    jmp isr_glue


global isr78
isr78:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 78
    jmp isr_glue


global isr79
isr79:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 79
    jmp isr_glue


global isr80
isr80:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 80
    jmp isr_glue


global isr81
isr81:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 81
    jmp isr_glue


global isr82
isr82:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 82
    jmp isr_glue


global isr83
isr83:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 83
    jmp isr_glue


global isr84
isr84:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 84
    jmp isr_glue


global isr85
isr85:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 85
    jmp isr_glue


global isr86
isr86:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 86
    jmp isr_glue


global isr87
isr87:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 87
    jmp isr_glue


global isr88
isr88:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 88
    jmp isr_glue


global isr89
isr89:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 89
    jmp isr_glue


global isr90
isr90:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 90
    jmp isr_glue


global isr91
isr91:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 91
    jmp isr_glue


global isr92
isr92:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 92
    jmp isr_glue


global isr93
isr93:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 93
    jmp isr_glue


global isr94
isr94:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 94
    jmp isr_glue


global isr95
isr95:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 95
    jmp isr_glue


global isr96
isr96:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 96
    jmp isr_glue


global isr97
isr97:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 97
    jmp isr_glue


global isr98
isr98:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 98
    jmp isr_glue


global isr99
isr99:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 99
    jmp isr_glue


global isr100
isr100:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 100
    jmp isr_glue


global isr101
isr101:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 101
    jmp isr_glue


global isr102
isr102:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 102
    jmp isr_glue


global isr103
isr103:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 103
    jmp isr_glue


global isr104
isr104:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 104
    jmp isr_glue


global isr105
isr105:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 105
    jmp isr_glue


global isr106
isr106:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 106
    jmp isr_glue


global isr107
isr107:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 107
    jmp isr_glue


global isr108
isr108:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 108
    jmp isr_glue


global isr109
isr109:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 109
    jmp isr_glue


global isr110
isr110:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 110
    jmp isr_glue


global isr111
isr111:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 111
    jmp isr_glue


global isr112
isr112:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 112
    jmp isr_glue


global isr113
isr113:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 113
    jmp isr_glue


global isr114
isr114:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 114
    jmp isr_glue


global isr115
isr115:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 115
    jmp isr_glue


global isr116
isr116:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 116
    jmp isr_glue


global isr117
isr117:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 117
    jmp isr_glue


global isr118
isr118:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 118
    jmp isr_glue


global isr119
isr119:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 119
    jmp isr_glue


global isr120
isr120:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 120
    jmp isr_glue


global isr121
isr121:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 121
    jmp isr_glue


global isr122
isr122:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 122
    jmp isr_glue


global isr123
isr123:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 123
    jmp isr_glue


global isr124
isr124:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 124
    jmp isr_glue


global isr125
isr125:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 125
    jmp isr_glue


global isr126
isr126:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 126
    jmp isr_glue


global isr127
isr127:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 127
    jmp isr_glue


global isr128
isr128:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 128
    jmp isr_glue


global isr129
isr129:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 129
    jmp isr_glue


global isr130
isr130:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 130
    jmp isr_glue


global isr131
isr131:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 131
    jmp isr_glue


global isr132
isr132:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 132
    jmp isr_glue


global isr133
isr133:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 133
    jmp isr_glue


global isr134
isr134:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 134
    jmp isr_glue


global isr135
isr135:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 135
    jmp isr_glue


global isr136
isr136:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 136
    jmp isr_glue


global isr137
isr137:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 137
    jmp isr_glue


global isr138
isr138:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 138
    jmp isr_glue


global isr139
isr139:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 139
    jmp isr_glue


global isr140
isr140:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 140
    jmp isr_glue


global isr141
isr141:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 141
    jmp isr_glue


global isr142
isr142:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 142
    jmp isr_glue


global isr143
isr143:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 143
    jmp isr_glue


global isr144
isr144:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 144
    jmp isr_glue


global isr145
isr145:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 145
    jmp isr_glue


global isr146
isr146:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 146
    jmp isr_glue


global isr147
isr147:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 147
    jmp isr_glue


global isr148
isr148:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 148
    jmp isr_glue


global isr149
isr149:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 149
    jmp isr_glue


global isr150
isr150:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 150
    jmp isr_glue


global isr151
isr151:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 151
    jmp isr_glue


global isr152
isr152:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 152
    jmp isr_glue


global isr153
isr153:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 153
    jmp isr_glue


global isr154
isr154:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 154
    jmp isr_glue


global isr155
isr155:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 155
    jmp isr_glue


global isr156
isr156:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 156
    jmp isr_glue


global isr157
isr157:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 157
    jmp isr_glue


global isr158
isr158:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 158
    jmp isr_glue


global isr159
isr159:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 159
    jmp isr_glue


global isr160
isr160:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 160
    jmp isr_glue


global isr161
isr161:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 161
    jmp isr_glue


global isr162
isr162:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 162
    jmp isr_glue


global isr163
isr163:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 163
    jmp isr_glue


global isr164
isr164:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 164
    jmp isr_glue


global isr165
isr165:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 165
    jmp isr_glue


global isr166
isr166:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 166
    jmp isr_glue


global isr167
isr167:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 167
    jmp isr_glue


global isr168
isr168:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 168
    jmp isr_glue


global isr169
isr169:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 169
    jmp isr_glue


global isr170
isr170:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 170
    jmp isr_glue


global isr171
isr171:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 171
    jmp isr_glue


global isr172
isr172:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 172
    jmp isr_glue


global isr173
isr173:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 173
    jmp isr_glue


global isr174
isr174:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 174
    jmp isr_glue


global isr175
isr175:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 175
    jmp isr_glue


global isr176
isr176:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 176
    jmp isr_glue


global isr177
isr177:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 177
    jmp isr_glue


global isr178
isr178:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 178
    jmp isr_glue


global isr179
isr179:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 179
    jmp isr_glue


global isr180
isr180:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 180
    jmp isr_glue


global isr181
isr181:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 181
    jmp isr_glue


global isr182
isr182:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 182
    jmp isr_glue


global isr183
isr183:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 183
    jmp isr_glue


global isr184
isr184:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 184
    jmp isr_glue


global isr185
isr185:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 185
    jmp isr_glue


global isr186
isr186:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 186
    jmp isr_glue


global isr187
isr187:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 187
    jmp isr_glue


global isr188
isr188:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 188
    jmp isr_glue


global isr189
isr189:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 189
    jmp isr_glue


global isr190
isr190:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 190
    jmp isr_glue


global isr191
isr191:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 191
    jmp isr_glue


global isr192
isr192:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 192
    jmp isr_glue


global isr193
isr193:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 193
    jmp isr_glue


global isr194
isr194:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 194
    jmp isr_glue


global isr195
isr195:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 195
    jmp isr_glue


global isr196
isr196:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 196
    jmp isr_glue


global isr197
isr197:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 197
    jmp isr_glue


global isr198
isr198:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 198
    jmp isr_glue


global isr199
isr199:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 199
    jmp isr_glue


global isr200
isr200:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 200
    jmp isr_glue


global isr201
isr201:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 201
    jmp isr_glue


global isr202
isr202:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 202
    jmp isr_glue


global isr203
isr203:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 203
    jmp isr_glue


global isr204
isr204:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 204
    jmp isr_glue


global isr205
isr205:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 205
    jmp isr_glue


global isr206
isr206:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 206
    jmp isr_glue


global isr207
isr207:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 207
    jmp isr_glue


global isr208
isr208:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 208
    jmp isr_glue


global isr209
isr209:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 209
    jmp isr_glue


global isr210
isr210:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 210
    jmp isr_glue


global isr211
isr211:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 211
    jmp isr_glue


global isr212
isr212:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 212
    jmp isr_glue


global isr213
isr213:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 213
    jmp isr_glue


global isr214
isr214:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 214
    jmp isr_glue


global isr215
isr215:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 215
    jmp isr_glue


global isr216
isr216:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 216
    jmp isr_glue


global isr217
isr217:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 217
    jmp isr_glue


global isr218
isr218:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 218
    jmp isr_glue


global isr219
isr219:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 219
    jmp isr_glue


global isr220
isr220:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 220
    jmp isr_glue


global isr221
isr221:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 221
    jmp isr_glue


global isr222
isr222:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 222
    jmp isr_glue


global isr223
isr223:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 223
    jmp isr_glue


global isr224
isr224:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 224
    jmp isr_glue


global isr225
isr225:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 225
    jmp isr_glue


global isr226
isr226:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 226
    jmp isr_glue


global isr227
isr227:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 227
    jmp isr_glue


global isr228
isr228:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 228
    jmp isr_glue


global isr229
isr229:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 229
    jmp isr_glue


global isr230
isr230:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 230
    jmp isr_glue


global isr231
isr231:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 231
    jmp isr_glue


global isr232
isr232:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 232
    jmp isr_glue


global isr233
isr233:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 233
    jmp isr_glue


global isr234
isr234:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 234
    jmp isr_glue


global isr235
isr235:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 235
    jmp isr_glue


global isr236
isr236:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 236
    jmp isr_glue


global isr237
isr237:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 237
    jmp isr_glue


global isr238
isr238:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 238
    jmp isr_glue


global isr239
isr239:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 239
    jmp isr_glue


global isr240
isr240:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 240
    jmp isr_glue


global isr241
isr241:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 241
    jmp isr_glue


global isr242
isr242:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 242
    jmp isr_glue


global isr243
isr243:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 243
    jmp isr_glue


global isr244
isr244:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 244
    jmp isr_glue


global isr245
isr245:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 245
    jmp isr_glue


global isr246
isr246:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 246
    jmp isr_glue


global isr247
isr247:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 247
    jmp isr_glue


global isr248
isr248:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 248
    jmp isr_glue


global isr249
isr249:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 249
    jmp isr_glue


global isr250
isr250:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 250
    jmp isr_glue


global isr251
isr251:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 251
    jmp isr_glue


global isr252
isr252:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 252
    jmp isr_glue


global isr253
isr253:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 253
    jmp isr_glue


global isr254
isr254:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 254
    jmp isr_glue


global isr255
isr255:
    push rsi ; placeholder for error code
    push rdi ; will be used to store irq number
    mov rdi, 255
    jmp isr_glue
;;%endif
