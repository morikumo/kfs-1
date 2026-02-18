/* Linker script (cf. https://wiki.osdev.org/Linker_Scripts) */
ENTRY(start)

SECTIONS
{
    . = 1M;             /* Kernel place a 1 Mo (en dessous = BIOS/video/ROM) */

    .text :             /* Code executable */
    {
        *(.multiboot)   /* Header multiboot EN PREMIER (GRUB scanne les 8 premiers Ko) */
        *(.text*)
    }

    .rodata : { *(.rodata*) }   /* Donnees en lecture seule (strings) */
    .data   : { *(.data*) }     /* Variables initialisees */
    .bss    : { *(.bss*) *(COMMON) }   /* Variables non-initialisees + pile */
}
