; Architecture i386
; NASM syntax

BITS 32

SECTION .multiboot
align 4
    dd 0x1BADB002        ; magic number multiboot
    dd 0x00              ; flags
    dd -(0x1BADB002)     ; checksum

SECTION .text
global start
extern kernel_main

start:
    cli                  ; disable interrupts
    mov esp, stack_top   ; setup stack
    call kernel_main
.hang:
    hlt                  ; stop CPU
    jmp .hang            ; loop if interrupt wakes CPU

SECTION .bss
align 16
stack_bottom:
    resb 16384           ; 16 KB stack
stack_top:
