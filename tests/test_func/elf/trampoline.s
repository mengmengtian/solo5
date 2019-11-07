section .text
    global trampoline
    trampoline:
        xor eax, eax
        mov r10, 1
        VMFUNC
        cmp rsi, 123
        jnz RT
        call 0x8fab
        mov ebx, eax
    RT:
        mov rcx, r10
        xor eax, eax
        VMFUNC
        ret
