/* Types custom (pas de <stdint.h> sans libc) */
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

/* 16 couleurs VGA, 4 bits chacune (cf. https://wiki.osdev.org/Text_UI#Colours) */
#define VGA_BLACK         0x0
#define VGA_BLUE          0x1
#define VGA_GREEN         0x2
#define VGA_CYAN          0x3
#define VGA_RED           0x4
#define VGA_MAGENTA       0x5
#define VGA_BROWN         0x6
#define VGA_LIGHT_GREY    0x7
#define VGA_DARK_GREY     0x8
#define VGA_LIGHT_BLUE    0x9
#define VGA_LIGHT_GREEN   0xA
#define VGA_LIGHT_CYAN    0xB
#define VGA_LIGHT_RED     0xC
#define VGA_LIGHT_MAGENTA 0xD
#define VGA_YELLOW        0xE
#define VGA_WHITE         0xF

#define VGA_COLOR(fg, bg) ((bg) << 4 | (fg))

/* Ecran VGA text mode 80x25 (cf. https://wiki.osdev.org/Text_UI)
 * Chaque case = 2 octets [couleur | caractere], base a 0xB8000 */
static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;
static uint8_t current_color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
static int cursor_pos = 0;

/* Ecrit un octet sur un port I/O x86 (necessaire pour le curseur hardware) */
static void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* Deplace le curseur clignotant (cf. https://wiki.osdev.org/Text_Mode_Cursor) */
static void update_cursor(void)
{
    outb(0x3D4, 14);                       /* Registre 14 : octet haut */
    outb(0x3D5, (uint8_t)(cursor_pos >> 8));
    outb(0x3D4, 15);                       /* Registre 15 : octet bas */
    outb(0x3D5, (uint8_t)(cursor_pos & 0xFF));
}

/* Decale tout l'ecran d'une ligne vers le haut, vide la derniere ligne */
static void scroll(void)
{
    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
        VGA_MEMORY[i] = VGA_MEMORY[i + VGA_WIDTH];
    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = ((uint16_t)current_color << 8) | ' ';
    cursor_pos -= VGA_WIDTH;
}

void set_color(uint8_t fg, uint8_t bg)
{
    current_color = VGA_COLOR(fg, bg);
}

/* Remplit l'ecran d'espaces pour effacer le texte laisse par GRUB */
void clear_screen(void)
{
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = ((uint16_t)current_color << 8) | ' ';
    cursor_pos = 0;
    update_cursor();
}

/* Ecrit une string dans la VRAM avec support \n et scroll */
void printk(const char *str)
{
    while (*str)
    {
        if (*str == '\n')
            cursor_pos += VGA_WIDTH - (cursor_pos % VGA_WIDTH);
        else
        {
            VGA_MEMORY[cursor_pos] = ((uint16_t)current_color << 8) | *str;
            cursor_pos++;
        }
        if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT)
            scroll();
        str++;
    }
    update_cursor();
}

/* Appelee par boot.asm apres la config de la pile */
void kernel_main(void)
{
    clear_screen();
    set_color(VGA_GREEN, VGA_BLACK);
    printk("42");
    while (1)
        __asm__("hlt");
}
