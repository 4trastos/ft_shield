section .data

h_init:
    dd 0x6a09e667
    dd 0xbb67ae85
    dd 0x3c6ef372
    dd 0xa54ff53a
    dd 0x510e527f
    dd 0x9b05688c
    dd 0x1f83d9ab
    dd 0x5be0cd19

k_const:
    dd 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5
    dd 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5
    dd 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3
    dd 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174
    dd 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc
    dd 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da
    dd 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7
    dd 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967
    dd 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13
    dd 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85
    dd 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3
    dd 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070
    dd 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5
    dd 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3
    dd 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208
    dd 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2

section .bss

ctx resb 108

section .text
global ft_sha256
global sha256_init
global ctx
global sha256_transform

sha256_init:
    push    rbp
    mov     rbp, rsp
    lea     rsi, [rel h_init]
    lea     rdi, [rel ctx]
    mov     rcx, 8
.loop:
    mov     eax, DWORD [rsi]
    mov     DWORD [rdi], eax
    add     rsi, 4
    add     rdi, 4
    dec     rcx
    jnz     .loop
    lea     rdi, [rel ctx + 96]
    mov     DWORD [rdi], 0
    mov     QWORD [rdi + 4], 0
    pop     rbp
    ret

sha256_transform:
    push    rbp
    mov     rbp, rsp
    push    r12
    push    r13
    push    r14
    push    r15
    push    rbx
    sub     rsp, 288

    ; W[0..15]
    lea     rsi, [rel ctx + 32]
    mov     rdi, rsp
    mov     rcx, 16
.build_w_0_15:
    mov     eax, DWORD [rsi]
    bswap   eax
    mov     DWORD [rdi], eax
    add     rsi, 4
    add     rdi, 4
    dec     rcx
    jnz     .build_w_0_15

    ; W[16..63]
    mov     rcx, 48
.build_w_16_63:
    mov     eax, DWORD [rdi - 15*4]
    mov     ebx, eax
    ror     eax, 7
    ror     ebx, 18
    xor     eax, ebx
    mov     ebx, DWORD [rdi - 15*4]
    shr     ebx, 3
    xor     eax, ebx
    mov     ebx, DWORD [rdi - 2*4]
    mov     edx, ebx
    ror     ebx, 17
    ror     edx, 19
    xor     ebx, edx
    mov     edx, DWORD [rdi - 2*4]
    shr     edx, 10
    xor     ebx, edx
    mov     edx, DWORD [rdi - 16*4]
    add     edx, eax
    add     edx, DWORD [rdi - 7*4]
    add     edx, ebx
    mov     DWORD [rdi], edx
    add     rdi, 4
    dec     rcx
    jnz     .build_w_16_63

    ; Cargar H en rsp+256
    lea     rsi, [rel ctx]
    lea     rdi, [rsp + 256]
    mov     rcx, 8
.load_h:
    mov     eax, DWORD [rsi]
    mov     DWORD [rdi], eax
    add     rsi, 4
    add     rdi, 4
    dec     rcx
    jnz     .load_h

    ; 64 rondas
    lea     rbx, [rel k_const]
    mov     r12, rsp
    mov     r13d, 64
.rounds:
    mov     eax, DWORD [rsp + 272]
    mov     edx, eax
    ror     eax, 6
    mov     ecx, edx
    ror     edx, 11
    xor     eax, edx
    ror     ecx, 25
    xor     eax, ecx

    mov     edx, DWORD [rsp + 272]
    mov     ecx, edx
    and     edx, DWORD [rsp + 276]
    not     ecx
    and     ecx, DWORD [rsp + 280]
    xor     edx, ecx

    mov     ecx, DWORD [rsp + 284]
    add     ecx, eax
    add     ecx, edx
    add     ecx, DWORD [rbx]
    add     ecx, DWORD [r12]

    mov     eax, DWORD [rsp + 256]
    mov     edx, eax
    ror     eax, 2
    mov     r14d, edx
    ror     edx, 13
    xor     eax, edx
    ror     r14d, 22
    xor     eax, r14d

    mov     edx, DWORD [rsp + 256]
    mov     r14d, edx
    and     edx, DWORD [rsp + 260]
    and     r14d, DWORD [rsp + 264]
    xor     edx, r14d
    mov     r14d, DWORD [rsp + 260]
    and     r14d, DWORD [rsp + 264]
    xor     edx, r14d

    add     eax, edx

    mov     r14d, DWORD [rsp + 280]
    mov     DWORD [rsp + 284], r14d
    mov     r14d, DWORD [rsp + 276]
    mov     DWORD [rsp + 280], r14d
    mov     r14d, DWORD [rsp + 272]
    mov     DWORD [rsp + 276], r14d
    mov     r14d, DWORD [rsp + 268]
    add     r14d, ecx
    mov     DWORD [rsp + 272], r14d
    mov     r14d, DWORD [rsp + 264]
    mov     DWORD [rsp + 268], r14d
    mov     r14d, DWORD [rsp + 260]
    mov     DWORD [rsp + 264], r14d
    mov     r14d, DWORD [rsp + 256]
    mov     DWORD [rsp + 260], r14d
    add     ecx, eax
    mov     DWORD [rsp + 256], ecx

    add     rbx, 4
    add     r12, 4
    dec     r13d
    jnz     .rounds

.update_h:
    lea     rsi, [rel ctx]
    lea     rdi, [rsp + 256]
    mov     rcx, 8
.update_loop:
    mov     eax, DWORD [rdi]
    add     DWORD [rsi], eax
    add     rsi, 4
    add     rdi, 4
    dec     rcx
    jnz     .update_loop

    add     rsp, 288
    pop     rbx
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbp
    ret

sha256_update:
    push    rbp
    mov     rbp, rsp
    push    r12
    push    r13
    push    r14
    push    rbx

    mov     r12, rdi
    mov     r13, rsi
    xor     r14, r14

.loop:
    cmp     r14, r13
    jge     .done

    lea     rdi, [rel ctx + 32]
    mov     eax, DWORD [rel ctx + 96]
    movzx   ecx, BYTE [r12 + r14]
    mov     BYTE [rdi + rax], cl

    inc     eax
    mov     DWORD [rel ctx + 96], eax

    cmp     eax, 64
    jne     .next
    call    sha256_transform

    mov     rax, QWORD [rel ctx + 100]
    add     rax, 512
    mov     QWORD [rel ctx + 100], rax
    mov     DWORD [rel ctx + 96], 0

.next:
    inc     r14
    jmp     .loop

.done:
    pop     rbx
    pop     r14
    pop     r13
    pop     r12
    pop     rbp
    ret

sha256_final:
    push    rbp
    mov     rbp, rsp
    push    r12
    push    r13
    push    r14
    push    r15

    mov     r12, rdx

    lea     rdi, [rel ctx + 32]
    mov     eax, DWORD [rel ctx + 96]
    mov     r13d, eax
    mov     BYTE [rdi + rax], 0x80
    inc     eax
    mov     DWORD [rel ctx + 96], eax

    cmp     eax, 56
    jle     .pad_zeros

.pad_extra:
    cmp     eax, 64
    jge     .do_extra_transform
    mov     BYTE [rdi + rax], 0
    inc     eax
    jmp     .pad_extra
.do_extra_transform:
    mov     DWORD [rel ctx + 96], eax
    call    sha256_transform
    mov     eax, 0
    mov     DWORD [rel ctx + 96], eax

.pad_zeros:
    lea     rdi, [rel ctx + 32]
    mov     eax, DWORD [rel ctx + 96]
.zero_loop:
    cmp     eax, 56
    jge     .write_length
    mov     BYTE [rdi + rax], 0
    inc     eax
    jmp     .zero_loop

.write_length:
    mov     eax, r13d
    xor     rbx, rbx
    mov     rbx, QWORD [rel ctx + 100]
    add     rax, rbx
    shl     rax, 3
    bswap   rax
    lea     rdi, [rel ctx + 32]
    mov     QWORD [rdi + 56], rax

    mov     eax, 64
    mov     DWORD [rel ctx + 96], eax
    call    sha256_transform

    lea     rsi, [rel ctx]
    mov     rcx, 8
    xor     r14, r14
.write_output:
    mov     eax, DWORD [rsi + r14*4]
    bswap   eax
    mov     DWORD [r12 + r14*4], eax
    inc     r14
    dec     rcx
    jnz     .write_output

    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbp
    ret

ft_sha256:
    push    rbp
    mov     rbp, rsp
    push    r12
    push    r13
    push    r14
    push    rbx

    mov     r12, rdi
    mov     r13, rsi
    mov     r14, rdx

    call    sha256_init

    mov     rdi, r12
    mov     rsi, r13
    call    sha256_update

    mov     rdx, r14
    call    sha256_final

    pop     rbx
    pop     r14
    pop     r13
    pop     r12
    pop     rbp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits