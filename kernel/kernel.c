typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

/* VGA colors */
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

static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;
static uint8_t current_color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
static int cursor_pos = 0;

void set_color(uint8_t fg, uint8_t bg)
{
    current_color = VGA_COLOR(fg, bg);
}

void clear_screen(void)
{
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = ((uint16_t)current_color << 8) | ' ';
    cursor_pos = 0;
}

void printk(const char *str)
{
    while (*str && cursor_pos < VGA_WIDTH * VGA_HEIGHT)
    {
        VGA_MEMORY[cursor_pos] = ((uint16_t)current_color << 8) | *str;
        cursor_pos++;
        str++;
    }
}

void kernel_main(void)
{
    clear_screen();
    set_color(VGA_GREEN, VGA_BLACK);
    printk("42");
    set_color(VGA_WHITE, VGA_BLACK);
    printk(" - kernel ready");
    while (1)
        __asm__("hlt");
}
