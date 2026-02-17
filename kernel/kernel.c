typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
static const uint8_t VGA_COLOR = 0x0F;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;
static int cursor_pos = 0;

void clear_screen(void)
{
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = ((uint16_t)VGA_COLOR << 8) | ' ';
    cursor_pos = 0;
}

void printstr(const char *str)
{
    while (*str && cursor_pos < VGA_WIDTH * VGA_HEIGHT)
    {
        VGA_MEMORY[cursor_pos] = ((uint16_t)VGA_COLOR << 8) | *str;
        cursor_pos++;
        str++;
    }
}

void kernel_main(void)
{
    clear_screen();
    printstr("42");
    while (1)
        __asm__("hlt");
}
