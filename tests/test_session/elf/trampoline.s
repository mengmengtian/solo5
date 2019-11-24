section .text
    global trampoline
    trampoline:
        xor eax, eax
	    VMFUNC
        cmp rsi, 0x1000
        jnz RT
        mov r10, rsp
        mov rax, r11
        shl rax, 18
        mov rsp, 0x1fffffff
        sub rsp, rax     
        call 0x50ef
        mov ebx, eax
        mov rsp, r10
    RT:
        mov rcx, r11
        xor eax, eax
        VMFUNC
        ret