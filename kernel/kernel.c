typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
static const uint8_t VGA_COLOR = 0x0F; // blanc sur noir

void kernel_main(void)
{
    // Nettoyage de l'écran (80 x 25)
    for (int i = 0; i < 80 * 25; i++)
    {
        VGA_MEMORY[i] = ((uint16_t)VGA_COLOR << 8) | ' ';
    }

    // Affichage de "42"
    VGA_MEMORY[0] = ((uint16_t)VGA_COLOR << 8) | '4';
    VGA_MEMORY[1] = ((uint16_t)VGA_COLOR << 8) | '2';

    while (1)
    {
        __asm__("hlt");
    }
}
