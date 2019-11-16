section .text
    global trampoline
    trampoline:
        mov r10, rsp
        mov rsp, 0x1effffff
        xor eax, eax
	VMFUNC
        cmp rsi, 0x1000
        jnz RT     
        call 0x8fe9
        mov ebx, eax
    RT:
        mov rcx, r11
        xor eax, eax
	mov rsp, r10
        VMFUNC
        ret
