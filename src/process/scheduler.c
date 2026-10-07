#include "scheduler.h"
#include "process.h"

extern void print(const char *str);
extern void put_char(char c);
extern void print_number(int number);

extern PCB process_table[MAX_PROCESSES];
extern int process_count;

void scheduler_run(void)
{
    int remaining[MAX_PROCESSES];

    int i;
    int finished = 0;
    int current;
    int execution_time;

    /* Copy the burst time of each process */
    for (i = 0; i < process_count; i++)
    {
        remaining[i] = process_table[i].burst_time;
    }

    print("\n");
    print("========================================\n");
    print("       ROUND ROBIN SCHEDULER\n");
    print("========================================\n\n");

    print("Time Quantum: ");
    print_number(TIME_QUANTUM);
    print("\n\n");

    print("PID   NAME       BURST\n");
    print("----------------------\n");

    for (i = 0; i < process_count; i++)
    {
        print_number(process_table[i].pid);
        print("     ");
        print(process_table[i].name);
        print("     ");
        print_number(process_table[i].burst_time);
        print("\n");
    }

    print("\nExecution Order:\n");

    /*
     * Round Robin scheduling simulation.
     * Each process gets CPU time equal to the time quantum.
     */
    while (finished < process_count)
    {
        for (current = 0; current < process_count; current++)
        {
            if (remaining[current] > 0)
            {
                process_table[current].state = RUNNING;

                print("P");
                print_number(process_table[current].pid);

                if (remaining[current] > TIME_QUANTUM)
                {
                    execution_time = TIME_QUANTUM;
                }
                else
                {
                    execution_time = remaining[current];
                }

                remaining[current] =
                    remaining[current] - execution_time;

                if (remaining[current] == 0)
                {
                    process_table[current].state = TERMINATED;
                    finished++;
                }
                else
                {
                    process_table[current].state = READY;
                }

                if (finished < process_count)
                {
                    print(" -> ");
                }
            }
        }
    }

    print("\n\n");
    print("Scheduling completed successfully.\n");
    print("All processes have completed.\n\n");

    /*
     * Reset the demonstration process table
     * so that ps shows the processes as READY again.
     */
    for (i = 0; i < process_count; i++)
    {
        process_table[i].state = READY;
    }
}
