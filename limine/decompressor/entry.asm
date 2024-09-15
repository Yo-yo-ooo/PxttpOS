extern bss_begin
extern bss_end
extern entry

section .entry progbits alloc exec nowrite align=16

global _start
_start:
    cld

    ; Zero out .bss
    xor al, al
    mov edi, bss_begin
    mov ecx, bss_end
    sub ecx, bss_begin
    rep stosb

    jmp entry

section .note.GNU-stack noalloc noexec nowrite progbits
