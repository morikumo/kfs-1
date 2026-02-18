BITS 32                         ; Mode 32 bits (GRUB nous y a mis)

; --- Header Multiboot (cf. https://wiki.osdev.org/Multiboot) ---
SECTION .multiboot
align 4
    dd 0x1BADB002               ; Magic number multiboot
    dd 0x00                     ; Flags
    dd -(0x1BADB002)            ; Checksum (magic + flags + checksum = 0)

; --- Code executable ---
SECTION .text
global start                    ; Visible par le linker (point d'entree)
extern kernel_main              ; Definie dans kernel.c

start:
    cli                         ; Desactive les interruptions (pas encore de gestionnaire)
    mov esp, stack_top          ; Configure la pile (necessaire pour appeler des fonctions)
    call kernel_main            ; Passe la main au C
.hang:
    hlt                         ; CPU en veille
    jmp .hang                   ; Boucle de securite si reveil inattendu

; --- Pile : 16 Ko, grandit vers le bas ---
SECTION .bss
align 16
stack_bottom:
    resb 16384
stack_top:
