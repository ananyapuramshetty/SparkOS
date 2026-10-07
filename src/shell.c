#include "shell.h"
#include "keyboard.h"

extern void print(const char *str);
extern void put_char(char c);

extern void process_list(void);
extern void process_show_states(void);
extern void scheduler_run(void);

#define BUFFER_SIZE 64

int string_equals(const char *a, const char *b)
{
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
        {
            return 0;
        }

        i++;
    }

    return a[i] == b[i];
}

void shell_help()
{
    print("\nAvailable commands:\n");
    print("  help      - Show available commands\n");
    print("  about     - Show Spark OS information\n");
    print("  clear     - Clear the screen\n");
    print("  echo      - Test echo command\n");
    print("  ps        - Show process table\n");
    print("  states    - Show process states\n");
    print("  schedule  - Run Round Robin scheduler\n");
    print("\n");
}

void shell_about()
{
    print("\nSpark OS v0.1\n");
    print("Educational Mini Operating System\n");
    print("Built as an OS learning project\n\n");
}

void shell_clear()
{
    extern void clear_screen();
    clear_screen();
}

void shell_echo()
{
    print("\nEcho command works!\n\n");
}

void shell_run()
{
    char command[BUFFER_SIZE];
    int position;

    print("SparkOS> ");

    while (1)
    {
        position = 0;
        command[0] = '\0';

        while (1)
        {
            char c = keyboard_getchar();

            if (c == '\n')
            {
                command[position] = '\0';
                put_char('\n');
                break;
            }

            if (c == '\b')
            {
                if (position > 0)
                {
                    position--;

                    extern int cursor_x;
                    extern int cursor_y;
                    extern volatile unsigned short *video_memory;

                    if (cursor_x > 0)
                    {
                        cursor_x--;
                    }

                    video_memory[cursor_y * 80 + cursor_x] =
                        (0x07 << 8) | ' ';
                }

                continue;
            }

            if (position < BUFFER_SIZE - 1)
            {
                command[position] = c;
                position++;
                command[position] = '\0';
                put_char(c);
            }
        }

        if (string_equals(command, "help"))
        {
            shell_help();
        }
        else if (string_equals(command, "about"))
        {
            shell_about();
        }
        else if (string_equals(command, "clear"))
        {
            shell_clear();
        }
        else if (string_equals(command, "echo"))
        {
            shell_echo();
        }
        else if (string_equals(command, "ps"))
        {
            process_list();
        }
        else if (string_equals(command, "states"))
        {
            process_show_states();
        }
        else if (string_equals(command, "schedule"))
        {
            scheduler_run();
        }
        else if (string_equals(command, ""))
        {
            /* Empty command */
        }
        else
        {
            print("Command not found: ");
            print(command);
            print("\n\n");
        }

        print("SparkOS> ");
    }
}
