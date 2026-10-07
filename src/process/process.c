#include "process.h"

/*
 * These functions are provided by the kernel.
 */
extern void print(const char *str);
extern void put_char(char c);


/*
 * Process table
 */
PCB process_table[MAX_PROCESSES];


/*
 * Number of currently created processes
 */
int process_count = 0;


/*
 * Function prototype
 */
void print_number(int number);


/*
 * Copy a string without using the C standard library.
 */
void copy_string(char *destination, const char *source)
{
    int i = 0;

    while (source[i] != '\0')
    {
        destination[i] = source[i];
        i++;
    }

    destination[i] = '\0';
}


/*
 * Convert process state to readable text.
 */
const char *state_name(process_state state)
{
    switch (state)
    {
        case NEW:
            return "NEW";

        case READY:
            return "READY";

        case RUNNING:
            return "RUNNING";

        case WAITING:
            return "WAITING";

        case TERMINATED:
            return "TERMINATED";

        default:
            return "UNKNOWN";
    }
}


/*
 * Initialize sample processes.
 */
void process_init(void)
{
    process_count = 3;


    /*
     * Process 1
     */
    process_table[0].pid = 1;

    copy_string(
        process_table[0].name,
        "Process1"
    );

    process_table[0].state = READY;
    process_table[0].priority = 1;
    process_table[0].burst_time = 5;


    /*
     * Process 2
     */
    process_table[1].pid = 2;

    copy_string(
        process_table[1].name,
        "Process2"
    );

    process_table[1].state = READY;
    process_table[1].priority = 1;
    process_table[1].burst_time = 3;


    /*
     * Process 3
     */
    process_table[2].pid = 3;

    copy_string(
        process_table[2].name,
        "Process3"
    );

    process_table[2].state = READY;
    process_table[2].priority = 1;
    process_table[2].burst_time = 4;
}


/*
 * Display all processes.
 */
void process_list(void)
{
    print("\n");
    print("PID   NAME       STATE       PRIORITY\n");
    print("-------------------------------------\n");

    for (int i = 0; i < process_count; i++)
    {
        print_number(process_table[i].pid);

        print("     ");

        print(process_table[i].name);

        print("     ");

        print(state_name(process_table[i].state));

        print("        ");

        print_number(process_table[i].priority);

        print("\n");
    }

    print("\n");
}


/*
 * Display the process state model.
 */
void process_show_states(void)
{
    print("\nProcess State Model:\n\n");

    print("NEW -> READY -> RUNNING\n");
    print("              |\n");
    print("              +-> WAITING -> READY\n");
    print("              |\n");
    print("              +-> TERMINATED\n");

    print("\n");
}


/*
 * Simple integer printing function.
 */
void print_number(int number)
{
    if (number >= 10)
    {
        put_char('0' + (number / 10));
        put_char('0' + (number % 10));
    }
    else
    {
        put_char('0' + number);
    }
}
