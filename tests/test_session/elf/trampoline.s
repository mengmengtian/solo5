section .text
    global trampoline
    trampoline:
        xor eax, eax
        mov edx, 0
        VMFUNC
        mov ecx, edx
        cmp rsi, 0x10101010
        jnz RT
        mov [0x108014], edx
        call 0x8fa3
        mov ebx, eax
        mov ecx, [0x108014]
    RT:
        xor eax, eax
        VMFUNC
        ret
