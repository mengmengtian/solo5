section .text
    global trampoline
    trampoline:
        xor eax, eax
        VMFUNC
        cmp rsi, 0x1000
        jnz RT
        call 0x8fb1
        mov ebx, eax
    RT:
        mov rcx, 1
        xor eax, eax
        VMFUNC
        ret
