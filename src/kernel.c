#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

volatile unsigned short *video_memory =
    (volatile unsigned short*)VIDEO_MEMORY;

int cursor_x = 0;
int cursor_y = 0;


/* -----------------------------------------
   Scroll screen up by one line
   ----------------------------------------- */

void scroll_screen()
{
    int row;
    int column;

    /* Move every row one position upward */
    for (row = 1; row < SCREEN_HEIGHT; row++)
    {
        for (column = 0; column < SCREEN_WIDTH; column++)
        {
            video_memory[
                (row - 1) * SCREEN_WIDTH + column
            ] =
                video_memory[
                    row * SCREEN_WIDTH + column
                ];
        }
    }

    /* Clear the last row */
    for (column = 0; column < SCREEN_WIDTH; column++)
    {
        video_memory[
            (SCREEN_HEIGHT - 1) * SCREEN_WIDTH + column
        ] = (0x07 << 8) | ' ';
    }

    cursor_y = SCREEN_HEIGHT - 1;
}


/* -----------------------------------------
   Clear entire screen
   ----------------------------------------- */

void clear_screen()
{
    int i;

    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        video_memory[i] = (0x07 << 8) | ' ';
    }

    cursor_x = 0;
    cursor_y = 0;
}


/* -----------------------------------------
   Put one character on screen
   ----------------------------------------- */

void put_char(char c)
{
    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= SCREEN_HEIGHT)
        {
            scroll_screen();
        }

        return;
    }


    if (c == '\b')
    {
        if (cursor_x > 0)
        {
            cursor_x--;

            video_memory[
                cursor_y * SCREEN_WIDTH + cursor_x
            ] = (0x07 << 8) | ' ';
        }

        return;
    }


    /* Display normal character */

    video_memory[
        cursor_y * SCREEN_WIDTH + cursor_x
    ] = (0x07 << 8) | c;

    cursor_x++;


    /* Move to next line when width is reached */

    if (cursor_x >= SCREEN_WIDTH)
    {
        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= SCREEN_HEIGHT)
        {
            scroll_screen();
        }
    }
}


/* -----------------------------------------
   Print a string
   ----------------------------------------- */

void print(const char *str)
{
    while (*str != '\0')
    {
        put_char(*str);
        str++;
    }
}


/* -----------------------------------------
   Start Shell
   ----------------------------------------- */

extern void shell_run();
extern void process_init();


void kernel_main()
{
    clear_screen();

    print("========================================\n");
    print("              SPARK OS\n");
    print("========================================\n\n");

    print("Spark OS v0.1\n");
    print("Kernel initialized successfully.\n");

    process_init();

    print("Process manager initialized.\n");
    print("System is running in QEMU.\n\n");

    shell_run();

    while (1)
    {
    }
}
