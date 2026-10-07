#include "keyboard.h"

static unsigned char keyboard_map[128] =
{
    0,  27,
    '1','2','3','4','5','6','7','8','9','0',
    '-','=',
    '\b',
    '\t',
    'q','w','e','r','t','y','u','i','o','p',
    '[',']',
    '\n',
    0,
    'a','s','d','f','g','h','j','k','l',
    ';','\'','`',
    0,
    '\\',
    'z','x','c','v','b','n','m',
    ',','.','/',
    0,
    '*',
    0,
    ' ',
};

static unsigned char inb(unsigned short port)
{
    unsigned char result;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );

    return result;
}

char keyboard_getchar()
{
    unsigned char scan_code;

    while (1)
    {
        while (!(inb(0x64) & 1))
        {
        }

        scan_code = inb(0x60);

        /* Ignore key release */
        if (scan_code & 0x80)
            continue;

        if (scan_code < 128)
        {
            char c = keyboard_map[scan_code];

            if (c != 0)
                return c;
        }
    }
}
