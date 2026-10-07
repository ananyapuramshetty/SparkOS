/*
 * ============================================================
 *                         SPARK OS
 *              OS Concepts Learning Environment
 * ============================================================
 *
 * Interactive Educational Methodology:
 *
 *                 THEORY
 *                    ↓
 *                 DIAGRAM
 *                    ↓
 *             PRACTICAL DEMO
 *                    ↓
 *                  RESULT
 *
 * The numbers displayed in menus are ONLY labels.
 * Users must type the command shown beside each topic.
 *
 * Example:
 *
 * 1. Process Management (use command "process")
 *
 * Then:
 *
 * SparkOS> process
 *
 * Compile:
 * gcc ubuntu_main.c -o sparkos -pthread
 *
 * Run:
 * ./sparkos
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#define MAX_PROCESSES 5
#define TIME_QUANTUM 2
#define COMMAND_SIZE 100

/* ============================================================
 *                    PROCESS STRUCTURES
 * ============================================================ */

typedef enum
{
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct
{
    int pid;
    char name[32];
    ProcessState state;
    int priority;
    int burst_time;
} PCB;

PCB process_table[MAX_PROCESSES];
int process_count = 3;

/* ============================================================
 *                    GENERAL FUNCTIONS
 * ============================================================ */

void print_line()
{
    printf("------------------------------------------------------------\n");
}

void print_header(const char *title)
{
    printf("\n");
    printf("============================================================\n");
    printf("                    %s\n", title);
    printf("============================================================\n");
}

void pause_screen()
{
    printf("\nPress ENTER to return...");
    getchar();
}

void clear_screen()
{
    printf("\033[2J\033[H");
}

void print_methodology(const char *topic)
{
    printf("\n");
    printf("INTERACTIVE LEARNING: %s\n", topic);
    printf("THEORY -> DIAGRAM -> PRACTICAL DEMO -> RESULT\n");
    printf("\n");
}

const char *state_name(ProcessState state)
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

/* ============================================================
 *                    PROCESS INITIALIZATION
 * ============================================================ */

void process_init()
{
    process_count = 3;

    process_table[0].pid = 1;
    strcpy(process_table[0].name, "Process1");
    process_table[0].state = READY;
    process_table[0].priority = 1;
    process_table[0].burst_time = 5;

    process_table[1].pid = 2;
    strcpy(process_table[1].name, "Process2");
    process_table[1].state = READY;
    process_table[1].priority = 1;
    process_table[1].burst_time = 3;

    process_table[2].pid = 3;
    strcpy(process_table[2].name, "Process3");
    process_table[2].state = READY;
    process_table[2].priority = 1;
    process_table[2].burst_time = 4;
}

/* ============================================================
 *                    PROCESS TABLE
 * ============================================================ */

void process_table_demo()
{
    int i;

    print_header("PROCESS TABLE");

    print_methodology("Process Table");

    printf("[THEORY]\n");
    printf("A Process Control Block (PCB) stores information about\n");
    printf("a process. Important information includes PID, process\n");
    printf("name, state, priority and CPU burst time.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("+--------------------------------------+\n");
    printf("|               PCB                    |\n");
    printf("+--------------------------------------+\n");
    printf("| PID                                  |\n");
    printf("| Process Name                         |\n");
    printf("| Process State                        |\n");
    printf("| Priority                             |\n");
    printf("| CPU Burst Time                       |\n");
    printf("+--------------------------------------+\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    printf("PID   NAME       STATE        PRIORITY   BURST\n");
    printf("------------------------------------------------\n");

    for (i = 0; i < process_count; i++)
    {
        printf("%-5d %-10s %-12s %-10d %d\n",
               process_table[i].pid,
               process_table[i].name,
               state_name(process_table[i].state),
               process_table[i].priority,
               process_table[i].burst_time);
    }

    printf("\n[RESULT]\n");
    printf("Process information was displayed from the process table.\n");

    pause_screen();
}

/* ============================================================
 *                    PROCESS STATES
 * ============================================================ */

void process_states_demo()
{
    print_header("PROCESS STATES");

    print_methodology("Process States");

    printf("[THEORY]\n");
    printf("A process moves through different states during its\n");
    printf("lifetime. The major states are NEW, READY, RUNNING,\n");
    printf("WAITING and TERMINATED.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("             +-------+\n");
    printf("             |  NEW  |\n");
    printf("             +---+---+\n");
    printf("                 |\n");
    printf("                 v\n");
    printf("             +-------+\n");
    printf("             | READY |\n");
    printf("             +---+---+\n");
    printf("                 |\n");
    printf("                 v\n");
    printf("            +---------+\n");
    printf("            | RUNNING |\n");
    printf("            +----+----+\n");
    printf("                 |\n");
    printf("          +------+------+\n");
    printf("          |             |\n");
    printf("          v             v\n");
    printf("     +---------+   +------------+\n");
    printf("     | WAITING |   | TERMINATED |\n");
    printf("     +----+----+   +------------+\n");
    printf("          |\n");
    printf("          +-------> READY\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    printf("Current process states:\n\n");

    for (int i = 0; i < process_count; i++)
    {
        printf("Process%d -> %s\n",
               process_table[i].pid,
               state_name(process_table[i].state));
    }

    printf("\n[RESULT]\n");
    printf("The process state lifecycle was demonstrated successfully.\n");

    pause_screen();
}

/* ============================================================
 *                    ROUND ROBIN
 * ============================================================ */

void scheduler_demo()
{
    int remaining[MAX_PROCESSES];
    int completed = 0;
    int current;
    int execution_time;
    int current_time = 0;

    for (int i = 0; i < process_count; i++)
    {
        remaining[i] = process_table[i].burst_time;
    }

    print_header("ROUND ROBIN SCHEDULER");

    print_methodology("Round Robin Scheduling");

    printf("[THEORY]\n");
    printf("Round Robin is a CPU scheduling technique in which\n");
    printf("each ready process receives a fixed amount of CPU time.\n");
    printf("This fixed amount is called the time quantum.\n");

    printf("\nTime Quantum = %d\n", TIME_QUANTUM);

    printf("\n[DIAGRAM]\n\n");

    printf("        READY QUEUE\n");
    printf("             |\n");
    printf("             v\n");
    printf("          +----+\n");
    printf("          | P1 |\n");
    printf("          +----+\n");
    printf("             |\n");
    printf("             v\n");
    printf("          +----+\n");
    printf("          | P2 |\n");
    printf("          +----+\n");
    printf("             |\n");
    printf("             v\n");
    printf("          +----+\n");
    printf("          | P3 |\n");
    printf("          +----+\n");
    printf("             |\n");
    printf("             +------> Repeat\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    printf("Process burst times:\n");

    for (int i = 0; i < process_count; i++)
    {
        printf("P%d = %d units\n",
               process_table[i].pid,
               process_table[i].burst_time);
    }

    printf("\nExecution timeline:\n");

    printf("0");

    while (completed < process_count)
    {
        for (current = 0; current < process_count; current++)
        {
            if (remaining[current] > 0)
            {
                process_table[current].state = RUNNING;

                if (remaining[current] > TIME_QUANTUM)
                {
                    execution_time = TIME_QUANTUM;
                }
                else
                {
                    execution_time = remaining[current];
                }

                printf(" --P%d-- %d",
                       process_table[current].pid,
                       current_time + execution_time);

                current_time += execution_time;

                remaining[current] -= execution_time;

                if (remaining[current] == 0)
                {
                    process_table[current].state = TERMINATED;
                    completed++;
                }
                else
                {
                    process_table[current].state = READY;
                }
            }
        }
    }

    printf("\n\nExecution order:\n");
    printf("P1 -> P2 -> P3 -> P1 -> P2 -> P3 -> P1\n");

    printf("\n[RESULT]\n");
    printf("Round Robin scheduling completed successfully.\n");
    printf("All processes completed execution.\n");

    for (int i = 0; i < process_count; i++)
    {
        process_table[i].state = READY;
    }

    pause_screen();
}

/* ============================================================
 *                    PROCESS CREATION
 * ============================================================ */

void fork_demo()
{
    pid_t pid;

    print_header("PROCESS CREATION");

    print_methodology("Process Creation");

    printf("[THEORY]\n");
    printf("The fork() system call creates a new child process.\n");
    printf("The parent process can wait for the child to finish.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("             Parent\n");
    printf("                |\n");
    printf("              fork()\n");
    printf("             /      \\\n");
    printf("            /        \\\n");
    printf("       Parent        Child\n");
    printf("          |            |\n");
    printf("          +-----+------+\n");
    printf("                |\n");
    printf("              wait()\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    printf("Current Process PID: %d\n", getpid());
    printf("Calling fork()...\n");

    pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return;
    }

    if (pid == 0)
    {
        printf("\nChild Process\n");
        printf("Child PID : %d\n", getpid());
        printf("Parent PID: %d\n", getppid());

        exit(0);
    }
    else
    {
        printf("\nParent Process\n");
        printf("Parent PID: %d\n", getpid());
        printf("Child PID : %d\n", pid);

        wait(NULL);

        printf("\nChild process execution completed.\n");
        printf("Parent waited for child process.\n");
    }

    printf("\n[RESULT]\n");
    printf("Process creation and synchronization completed.\n");

    pause_screen();
}

/* ============================================================
 *                         PIPE
 * ============================================================ */

void pipe_demo()
{
    int fd[2];
    pid_t pid;

    char message[] = "Hello from parent through pipe!";
    char buffer[100];

    print_header("PIPE COMMUNICATION");

    print_methodology("Pipe Communication");

    printf("[THEORY]\n");
    printf("A pipe provides a communication channel between processes.\n");
    printf("One process writes data and another process reads it.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("+----------+       PIPE       +----------+\n");
    printf("| WRITER   |  ------------->  | READER   |\n");
    printf("| Process  |                  | Process  |\n");
    printf("+----------+                  +----------+\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    if (pipe(fd) == -1)
    {
        perror("pipe");
        return;
    }

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        close(fd[1]);

        int bytes = read(fd[0],
                         buffer,
                         sizeof(buffer) - 1);

        if (bytes >= 0)
        {
            buffer[bytes] = '\0';

            printf("Child received: %s\n",
                   buffer);
        }

        close(fd[0]);

        exit(0);
    }
    else
    {
        close(fd[0]);

        write(fd[1],
              message,
              strlen(message));

        close(fd[1]);

        wait(NULL);

        printf("Parent sent: %s\n",
               message);
    }

    printf("\n[RESULT]\n");
    printf("Pipe communication completed successfully.\n");

    pause_screen();
}

/* ============================================================
 *                         FIFO
 * ============================================================ */

void fifo_demo()
{
    const char *fifo_name = "sparkos_fifo";

    char message[] = "Hello through named FIFO!";
    char buffer[100];

    print_header("NAMED FIFO");

    print_methodology("Named FIFO Communication");

    printf("[THEORY]\n");
    printf("A named FIFO is a special file that provides a named\n");
    printf("communication channel between processes.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("+----------+       FIFO       +----------+\n");
    printf("| WRITER   |  ------------->  | READER   |\n");
    printf("| Process  |                  | Process  |\n");
    printf("+----------+                  +----------+\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    unlink(fifo_name);

    if (mkfifo(fifo_name, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo");
            return;
        }
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        unlink(fifo_name);
        return;
    }

    if (pid == 0)
    {
        int fd = open(fifo_name, O_RDONLY);

        if (fd < 0)
        {
            perror("FIFO open");
            exit(1);
        }

        int bytes = read(fd,
                         buffer,
                         sizeof(buffer) - 1);

        if (bytes >= 0)
        {
            buffer[bytes] = '\0';

            printf("Reader received: %s\n",
                   buffer);
        }

        close(fd);

        exit(0);
    }
    else
    {
        sleep(1);

        int fd = open(fifo_name,
                      O_WRONLY);

        if (fd < 0)
        {
            perror("FIFO open");
            unlink(fifo_name);
            return;
        }

        write(fd,
              message,
              strlen(message));

        close(fd);

        wait(NULL);

        unlink(fifo_name);

        printf("Writer sent: %s\n",
               message);
    }

    printf("\n[RESULT]\n");
    printf("Named FIFO communication completed successfully.\n");

    pause_screen();
}

/* ============================================================
 *                         SIGNAL
 * ============================================================ */

volatile sig_atomic_t signal_received = 0;

void spark_signal_handler(int signal_number)
{
    signal_received = signal_number;
}

void signal_demo()
{
    print_header("SIGNAL COMMUNICATION");

    print_methodology("Signals");

    printf("[THEORY]\n");
    printf("A signal is a software notification sent to a process.\n");
    printf("It allows a process to respond to an event.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("Sender Process\n");
    printf("      |\n");
    printf("      | SIGUSR1\n");
    printf("      v\n");
    printf("Receiver Process\n");
    printf("      |\n");
    printf("Signal Handler\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    signal(SIGUSR1,
           spark_signal_handler);

    printf("Current PID: %d\n",
           getpid());

    printf("Sending SIGUSR1...\n");

    raise(SIGUSR1);

    if (signal_received == SIGUSR1)
    {
        printf("Signal handler received SIGUSR1.\n");
    }

    printf("\n[RESULT]\n");
    printf("Signal communication completed successfully.\n");

    pause_screen();
}

/* ============================================================
 *                    MEMORY MANAGEMENT
 * ============================================================ */

int global_data = 100;
int global_bss;

void memory_layout_demo()
{
    int stack_variable = 10;
    int *heap_variable;

    heap_variable = malloc(sizeof(int));

    if (heap_variable == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    *heap_variable = 20;

    print_header("MEMORY LAYOUT");

    print_methodology("Memory Layout");

    printf("[THEORY]\n");
    printf("A running program uses different memory regions.\n");
    printf("Important regions include text, data, BSS, heap and stack.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("High Address\n");
    printf("-------------\n");
    printf("|   STACK   |\n");
    printf("-------------\n");
    printf("|    HEAP   |\n");
    printf("-------------\n");
    printf("|    BSS    |\n");
    printf("-------------\n");
    printf("|   DATA    |\n");
    printf("-------------\n");
    printf("|   TEXT    |\n");
    printf("-------------\n");
    printf("Low Address\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    printf("Text address : %p\n",
           (void *)memory_layout_demo);

    printf("Data address : %p\n",
           (void *)&global_data);

    printf("BSS address  : %p\n",
           (void *)&global_bss);

    printf("Heap address : %p\n",
           (void *)heap_variable);

    printf("Stack address: %p\n",
           (void *)&stack_variable);

    free(heap_variable);

    printf("\n[RESULT]\n");
    printf("Major memory regions were demonstrated successfully.\n");

    pause_screen();
}

/* ============================================================
 *                         MALLOC
 * ============================================================ */

void malloc_demo()
{
    int *numbers;

    print_header("DYNAMIC MEMORY ALLOCATION");

    print_methodology("Dynamic Memory Allocation");

    printf("[THEORY]\n");
    printf("malloc() dynamically allocates memory from the heap.\n");
    printf("free() releases the allocated memory.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("Program\n");
    printf("   |\n");
    printf("   | malloc()\n");
    printf("   v\n");
    printf("+----------------+\n");
    printf("|      HEAP      |\n");
    printf("| allocated data |\n");
    printf("+----------------+\n");
    printf("   |\n");
    printf("   | free()\n");
    printf("   v\n");
    printf("Memory released\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    numbers = malloc(5 * sizeof(int));

    if (numbers == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        numbers[i] = (i + 1) * 10;
    }

    printf("Allocated memory address: %p\n",
           (void *)numbers);

    printf("Stored values: ");

    for (int i = 0; i < 5; i++)
    {
        printf("%d ",
               numbers[i]);
    }

    printf("\n");

    free(numbers);

    printf("Memory released using free().\n");

    printf("\n[RESULT]\n");
    printf("Dynamic memory allocation completed successfully.\n");

    pause_screen();
}

/* ============================================================
 *                         FILE I/O
 * ============================================================ */

void files_demo()
{
    FILE *file;
    char buffer[200];

    print_header("FILE INPUT / OUTPUT");

    print_methodology("File Input and Output");

    printf("[THEORY]\n");
    printf("Operating systems provide file operations such as\n");
    printf("create, open, read, write and close.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("Application\n");
    printf("    |\n");
    printf("    v\n");
    printf("   open()\n");
    printf("    |\n");
    printf("    v\n");
    printf(" +--------+\n");
    printf(" |  FILE  |\n");
    printf(" +--------+\n");
    printf("    |\n");
    printf(" read/write\n");
    printf("    |\n");
    printf("    v\n");
    printf("  close()\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    file = fopen("sparkos_demo.txt",
                 "w");

    if (file == NULL)
    {
        perror("fopen");
        return;
    }

    fprintf(file,
            "Spark OS file system demonstration.\n");

    fprintf(file,
            "File operations are working successfully.\n");

    fclose(file);

    file = fopen("sparkos_demo.txt",
                 "r");

    if (file == NULL)
    {
        perror("fopen");
        return;
    }

    printf("File contents:\n");

    while (fgets(buffer,
                 sizeof(buffer),
                 file) != NULL)
    {
        printf("%s",
               buffer);
    }

    fclose(file);

    remove("sparkos_demo.txt");

    printf("\n[RESULT]\n");
    printf("File creation, writing, reading and closing completed.\n");

    pause_screen();
}

/* ============================================================
 *                    FILE DESCRIPTORS
 * ============================================================ */

void fd_demo()
{
    int fd;

    const char *message =
        "Spark OS file descriptor demonstration.\n";

    print_header("FILE DESCRIPTORS");

    print_methodology("File Descriptors");

    printf("[THEORY]\n");
    printf("A file descriptor is a small integer used by the\n");
    printf("operating system to identify an open file or I/O resource.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("Application\n");
    printf("    |\n");
    printf("    v\n");
    printf("File Descriptor\n");
    printf("    |\n");
    printf("    v\n");
    printf(" Open File\n");
    printf("    |\n");
    printf("    v\n");
    printf("Storage / Device\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    fd = open("sparkos_fd.txt",
              O_CREAT | O_WRONLY | O_TRUNC,
              0666);

    if (fd < 0)
    {
        perror("open");
        return;
    }

    printf("File descriptor returned by open(): %d\n",
           fd);

    write(fd,
          message,
          strlen(message));

    close(fd);

    printf("File descriptor closed successfully.\n");

    remove("sparkos_fd.txt");

    printf("\n[RESULT]\n");
    printf("File descriptor operations completed successfully.\n");

    pause_screen();
}

/* ============================================================
 *                          INODE
 * ============================================================ */

void inode_demo()
{
    struct stat file_info;

    const char *filename =
        "sparkos_inode.txt";

    FILE *file;

    print_header("INODE INFORMATION");

    print_methodology("File Metadata and Inode");

    printf("[THEORY]\n");
    printf("An inode stores metadata about a file in Unix-like\n");
    printf("file systems. It contains information such as file\n");
    printf("size, permissions and inode number.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("+----------------+\n");
    printf("|   File Name    |\n");
    printf("+--------+-------+\n");
    printf("         |\n");
    printf("         v\n");
    printf("+----------------+\n");
    printf("|     INODE      |\n");
    printf("+----------------+\n");
    printf("| Size           |\n");
    printf("| Permissions    |\n");
    printf("| Inode Number   |\n");
    printf("| Timestamps     |\n");
    printf("+----------------+\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    file = fopen(filename,
                 "w");

    if (file == NULL)
    {
        perror("fopen");
        return;
    }

    fprintf(file,
            "Spark OS inode demonstration.\n");

    fclose(file);

    if (stat(filename,
             &file_info) == -1)
    {
        perror("stat");
        remove(filename);
        return;
    }

    printf("File name    : %s\n",
           filename);

    printf("Inode number : %lu\n",
           (unsigned long)file_info.st_ino);

    printf("File size    : %ld bytes\n",
           (long)file_info.st_size);

    printf("Permissions  : %o\n",
           file_info.st_mode & 0777);

    remove(filename);

    printf("\n[RESULT]\n");
    printf("File metadata and inode information displayed successfully.\n");

    pause_screen();
}

/* ============================================================
 *                       THREAD CREATION
 * ============================================================ */

void *thread_function(void *arg)
{
    int thread_number = *(int *)arg;

    printf("Thread %d is running.\n",
           thread_number);

    return NULL;
}

void threads_demo()
{
    pthread_t thread1;
    pthread_t thread2;

    int number1 = 1;
    int number2 = 2;

    print_header("THREAD CREATION");

    print_methodology("Thread Creation");

    printf("[THEORY]\n");
    printf("A thread is a lightweight unit of execution inside\n");
    printf("a process. Multiple threads can execute within a process.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("+----------------------+\n");
    printf("|       PROCESS        |\n");
    printf("+----------+-----------+\n");
    printf("           |\n");
    printf("     +-----+-----+\n");
    printf("     |           |\n");
    printf("   Thread 1    Thread 2\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    pthread_create(&thread1,
                   NULL,
                   thread_function,
                   &number1);

    pthread_create(&thread2,
                   NULL,
                   thread_function,
                   &number2);

    pthread_join(thread1,
                 NULL);

    pthread_join(thread2,
                 NULL);

    printf("\n[RESULT]\n");
    printf("Multiple threads were created and joined successfully.\n");

    pause_screen();
}

/* ============================================================
 *                           MUTEX
 * ============================================================ */

int shared_counter = 0;
pthread_mutex_t counter_mutex;

void *mutex_worker(void *arg)
{
    (void)arg;

    for (int i = 0; i < 10000; i++)
    {
        pthread_mutex_lock(&counter_mutex);

        shared_counter++;

        pthread_mutex_unlock(&counter_mutex);
    }

    return NULL;
}

void mutex_demo()
{
    pthread_t thread1;
    pthread_t thread2;

    shared_counter = 0;

    print_header("MUTEX SYNCHRONIZATION");

    print_methodology("Mutex Synchronization");

    printf("[THEORY]\n");
    printf("A mutex provides mutual exclusion. It prevents multiple\n");
    printf("threads from modifying shared data at the same time.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("Thread 1 ----+\n");
    printf("             |\n");
    printf("             v\n");
    printf("          +-------+\n");
    printf("          | MUTEX |\n");
    printf("          +---+---+\n");
    printf("              |\n");
    printf("              v\n");
    printf("       Shared Counter\n");
    printf("              ^\n");
    printf("              |\n");
    printf("          +---+---+\n");
    printf("          | MUTEX |\n");
    printf("          +-------+\n");
    printf("              ^\n");
    printf("              |\n");
    printf("          Thread 2\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    pthread_mutex_init(&counter_mutex,
                       NULL);

    pthread_create(&thread1,
                   NULL,
                   mutex_worker,
                   NULL);

    pthread_create(&thread2,
                   NULL,
                   mutex_worker,
                   NULL);

    pthread_join(thread1,
                 NULL);

    pthread_join(thread2,
                 NULL);

    pthread_mutex_destroy(&counter_mutex);

    printf("Final shared counter: %d\n",
           shared_counter);

    printf("\n[RESULT]\n");
    printf("Mutex successfully protected the shared resource.\n");

    pause_screen();
}

/* ============================================================
 *                         SEMAPHORE
 * ============================================================ */

sem_t resource_semaphore;

void *semaphore_worker(void *arg)
{
    int id = *(int *)arg;

    sem_wait(&resource_semaphore);

    printf("Thread %d entered the protected resource.\n",
           id);

    sleep(1);

    printf("Thread %d leaving the protected resource.\n",
           id);

    sem_post(&resource_semaphore);

    return NULL;
}

void semaphore_demo()
{
    pthread_t threads[3];

    int ids[3] = {1, 2, 3};

    print_header("SEMAPHORE SYNCHRONIZATION");

    print_methodology("Semaphore Synchronization");

    printf("[THEORY]\n");
    printf("A semaphore controls access to a shared resource\n");
    printf("using a counter. It can limit how many threads enter\n");
    printf("a protected resource at one time.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("Thread 1 ----+\n");
    printf("Thread 2 ----+----> SEMAPHORE ----> RESOURCE\n");
    printf("Thread 3 ----+\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    sem_init(&resource_semaphore,
             0,
             1);

    for (int i = 0; i < 3; i++)
    {
        pthread_create(&threads[i],
                       NULL,
                       semaphore_worker,
                       &ids[i]);
    }

    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i],
                     NULL);
    }

    sem_destroy(&resource_semaphore);

    printf("\n[RESULT]\n");
    printf("Semaphore successfully controlled resource access.\n");

    pause_screen();
}

/* ============================================================
 *                       SYSTEM CALL
 * ============================================================ */

void syscall_demo()
{
    print_header("SYSTEM CALL DEMONSTRATION");

    print_methodology("System Calls");

    printf("[THEORY]\n");
    printf("A system call allows a user program to request a\n");
    printf("service from the operating system kernel.\n");

    printf("\n[DIAGRAM]\n\n");

    printf("User Program\n");
    printf("     |\n");
    printf("     | System Call\n");
    printf("     v\n");
    printf("+-----------+\n");
    printf("|   Kernel  |\n");
    printf("+-----------+\n");
    printf("     |\n");
    printf("     v\n");
    printf(" OS Service\n");

    printf("\n[PRACTICAL DEMO]\n\n");

    printf("Calling getpid()...\n");

    printf("Operating System returned PID: %d\n",
           getpid());

    printf("\n[RESULT]\n");
    printf("System call demonstration completed successfully.\n");

    pause_screen();
}

/* ============================================================
 *                          ABOUT
 * ============================================================ */

void about_sparkos()
{
    print_header("ABOUT SPARK OS");

    printf("Spark OS\n");
    printf("OS Concepts Learning Environment\n\n");

    printf("Spark OS connects operating-system theory with\n");
    printf("practical demonstrations through an interactive shell.\n");

    printf("\nOur educational methodology is:\n\n");

    printf("       THEORY\n");
    printf("          |\n");
    printf("          v\n");
    printf("       DIAGRAM\n");
    printf("          |\n");
    printf("          v\n");
    printf("   PRACTICAL DEMO\n");
    printf("          |\n");
    printf("          v\n");
    printf("        RESULT\n");

    printf("\nThe purpose is to help students understand OS concepts\n");
    printf("by seeing both the concept and its practical execution.\n");

    pause_screen();
}

/* ============================================================
 *                           HELP
 * ============================================================ */

void show_help()
{
    print_header("SPARK OS HELP");

    printf("IMPORTANT:\n");
    printf("Spark OS is command based.\n");
    printf("The numbers shown on menus are only labels.\n");
    printf("Always type the command written in brackets.\n");

    printf("\nMAIN COMMANDS\n");
    print_line();

    printf("1. Process Management\n");
    printf("   (use command \"process\")\n\n");

    printf("2. Inter-Process Communication\n");
    printf("   (use command \"ipc\")\n\n");

    printf("3. Memory Management\n");
    printf("   (use command \"memory\")\n\n");

    printf("4. File Management\n");
    printf("   (use command \"files\")\n\n");

    printf("5. Threads & Synchronization\n");
    printf("   (use command \"threads\")\n\n");

    printf("6. System Calls\n");
    printf("   (use command \"syscall\")\n\n");

    printf("7. About Spark OS\n");
    printf("   (use command \"about\")\n\n");

    printf("8. Help\n");
    printf("   (use command \"help\")\n\n");

    printf("9. Exit\n");
    printf("   (use command \"exit\")\n");

    printf("\nDIRECT COMMANDS\n");
    print_line();

    printf("ps         -> Process table\n");
    printf("states     -> Process states\n");
    printf("schedule   -> Round Robin scheduler\n");
    printf("create     -> Process creation\n");
    printf("pipe       -> Pipe communication\n");
    printf("fifo       -> Named FIFO\n");
    printf("signal     -> Signal communication\n");
    printf("layout     -> Memory layout\n");
    printf("malloc     -> Dynamic memory allocation\n");
    printf("io         -> File I/O\n");
    printf("fd         -> File descriptors\n");
    printf("inode      -> Inode information\n");
    printf("thread     -> Thread creation\n");
    printf("mutex      -> Mutex synchronization\n");
    printf("semaphore  -> Semaphore synchronization\n");

    printf("\nLEARNING FLOW\n");
    print_line();

    printf("Every major demonstration follows:\n\n");

    printf("THEORY\n");
    printf("   |\n");
    printf("   v\n");
    printf("DIAGRAM\n");
    printf("   |\n");
    printf("   v\n");
    printf("PRACTICAL DEMO\n");
    printf("   |\n");
    printf("   v\n");
    printf("RESULT\n");
}

/* ============================================================
 *                  PROCESS MODULE
 * ============================================================ */

void process_module()
{
    char command[COMMAND_SIZE];

    print_header("PROCESS MANAGEMENT");

    printf("Understand how processes are stored, moved,\n");
    printf("scheduled and created.\n\n");

    printf("Type the command shown beside the topic.\n\n");

    printf("1. Process Table\n");
    printf("   (use command \"table\")\n");
    printf("   -> Shows how process information is stored.\n\n");

    printf("2. Process States\n");
    printf("   (use command \"states\")\n");
    printf("   -> Shows the lifecycle of a process.\n\n");

    printf("3. Round Robin Scheduler\n");
    printf("   (use command \"schedule\")\n");
    printf("   -> Shows how CPU time is shared.\n\n");

    printf("4. Process Creation\n");
    printf("   (use command \"create\")\n");
    printf("   -> Shows how a child process is created.\n\n");

    printf("5. Back\n");
    printf("   (use command \"back\")\n");

    printf("\nExample:\n");
    printf("SparkOS/Process> states\n");

    while (1)
    {
        printf("\nSparkOS/Process> ");

        if (fgets(command,
                  sizeof(command),
                  stdin) == NULL)
        {
            return;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "table") == 0)
        {
            process_table_demo();
        }
        else if (strcmp(command, "states") == 0)
        {
            process_states_demo();
        }
        else if (strcmp(command, "schedule") == 0)
        {
            scheduler_demo();
        }
        else if (strcmp(command, "create") == 0)
        {
            fork_demo();
        }
        else if (strcmp(command, "back") == 0)
        {
            return;
        }
        else if (strcmp(command, "help") == 0)
        {
            printf("\nAvailable Process commands:\n");
            printf("table     -> Process Table\n");
            printf("states    -> Process States\n");
            printf("schedule  -> Round Robin Scheduler\n");
            printf("create    -> Process Creation\n");
            printf("back      -> Main Shell\n");
        }
        else if (strcmp(command, "clear") == 0)
        {
            clear_screen();
        }
        else if (strlen(command) == 0)
        {
            continue;
        }
        else
        {
            printf("\nUnknown command: %s\n",
                   command);

            printf("Type the command shown beside the topic.\n");
        }
    }
}

/* ============================================================
 *                       IPC MODULE
 * ============================================================ */

void ipc_module()
{
    char command[COMMAND_SIZE];

    print_header("INTER-PROCESS COMMUNICATION");

    printf("Learn how processes communicate with each other.\n\n");

    printf("Type the command shown beside the topic.\n\n");

    printf("1. Pipe Communication\n");
    printf("   (use command \"pipe\")\n");
    printf("   -> Demonstrates communication through a pipe.\n\n");

    printf("2. Named FIFO\n");
    printf("   (use command \"fifo\")\n");
    printf("   -> Demonstrates named process communication.\n\n");

    printf("3. Signals\n");
    printf("   (use command \"signal\")\n");
    printf("   -> Demonstrates software notifications.\n\n");

    printf("4. Back\n");
    printf("   (use command \"back\")\n");

    printf("\nExample:\n");
    printf("SparkOS/IPC> pipe\n");

    while (1)
    {
        printf("\nSparkOS/IPC> ");

        if (fgets(command,
                  sizeof(command),
                  stdin) == NULL)
        {
            return;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "pipe") == 0)
        {
            pipe_demo();
        }
        else if (strcmp(command, "fifo") == 0)
        {
            fifo_demo();
        }
        else if (strcmp(command, "signal") == 0)
        {
            signal_demo();
        }
        else if (strcmp(command, "back") == 0)
        {
            return;
        }
        else if (strcmp(command, "help") == 0)
        {
            printf("\nAvailable IPC commands:\n");
            printf("pipe      -> Pipe\n");
            printf("fifo      -> Named FIFO\n");
            printf("signal    -> Signals\n");
            printf("back      -> Main Shell\n");
        }
        else if (strcmp(command, "clear") == 0)
        {
            clear_screen();
        }
        else if (strlen(command) == 0)
        {
            continue;
        }
        else
        {
            printf("\nUnknown command: %s\n",
                   command);

            printf("Type the command shown beside the topic.\n");
        }
    }
}

/* ============================================================
 *                     MEMORY MODULE
 * ============================================================ */

void memory_module()
{
    char command[COMMAND_SIZE];

    print_header("MEMORY MANAGEMENT");

    printf("Learn how programs use and allocate memory.\n\n");

    printf("Type the command shown beside the topic.\n\n");

    printf("1. Memory Layout\n");
    printf("   (use command \"layout\")\n");
    printf("   -> Shows text, data, BSS, heap and stack.\n\n");

    printf("2. Dynamic Memory Allocation\n");
    printf("   (use command \"malloc\")\n");
    printf("   -> Demonstrates malloc() and free().\n\n");

    printf("3. Back\n");
    printf("   (use command \"back\")\n");

    printf("\nExample:\n");
    printf("SparkOS/Memory> layout\n");

    while (1)
    {
        printf("\nSparkOS/Memory> ");

        if (fgets(command,
                  sizeof(command),
                  stdin) == NULL)
        {
            return;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "layout") == 0)
        {
            memory_layout_demo();
        }
        else if (strcmp(command, "malloc") == 0)
        {
            malloc_demo();
        }
        else if (strcmp(command, "back") == 0)
        {
            return;
        }
        else if (strcmp(command, "help") == 0)
        {
            printf("\nAvailable Memory commands:\n");
            printf("layout    -> Memory Layout\n");
            printf("malloc    -> Dynamic Memory\n");
            printf("back      -> Main Shell\n");
        }
        else if (strcmp(command, "clear") == 0)
        {
            clear_screen();
        }
        else if (strlen(command) == 0)
        {
            continue;
        }
        else
        {
            printf("\nUnknown command: %s\n",
                   command);

            printf("Type the command shown beside the topic.\n");
        }
    }
}

/* ============================================================
 *                      FILE MODULE
 * ============================================================ */

void files_module()
{
    char command[COMMAND_SIZE];

    print_header("FILE MANAGEMENT");

    printf("Learn how operating systems handle files and metadata.\n\n");

    printf("Type the command shown beside the topic.\n\n");

    printf("1. File Input / Output\n");
    printf("   (use command \"io\")\n");
    printf("   -> Demonstrates file creation, writing and reading.\n\n");

    printf("2. File Descriptors\n");
    printf("   (use command \"fd\")\n");
    printf("   -> Demonstrates low-level file access.\n\n");

    printf("3. Inode Information\n");
    printf("   (use command \"inode\")\n");
    printf("   -> Demonstrates file metadata.\n\n");

    printf("4. Back\n");
    printf("   (use command \"back\")\n");

    printf("\nExample:\n");
    printf("SparkOS/Files> inode\n");

    while (1)
    {
        printf("\nSparkOS/Files> ");

        if (fgets(command,
                  sizeof(command),
                  stdin) == NULL)
        {
            return;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "io") == 0)
        {
            files_demo();
        }
        else if (strcmp(command, "fd") == 0)
        {
            fd_demo();
        }
        else if (strcmp(command, "inode") == 0)
        {
            inode_demo();
        }
        else if (strcmp(command, "back") == 0)
        {
            return;
        }
        else if (strcmp(command, "help") == 0)
        {
            printf("\nAvailable File commands:\n");
            printf("io        -> File I/O\n");
            printf("fd        -> File Descriptors\n");
            printf("inode     -> Inode Information\n");
            printf("back      -> Main Shell\n");
        }
        else if (strcmp(command, "clear") == 0)
        {
            clear_screen();
        }
        else if (strlen(command) == 0)
        {
            continue;
        }
        else
        {
            printf("\nUnknown command: %s\n",
                   command);

            printf("Type the command shown beside the topic.\n");
        }
    }
}

/* ============================================================
 *                 THREADS MODULE
 * ============================================================ */

void threads_module()
{
    char command[COMMAND_SIZE];

    print_header("THREADS & SYNCHRONIZATION");

    printf("Learn how threads execute and share resources safely.\n\n");

    printf("Type the command shown beside the topic.\n\n");

    printf("1. Thread Creation\n");
    printf("   (use command \"create\")\n");
    printf("   -> Demonstrates creation of multiple threads.\n\n");

    printf("2. Mutex Synchronization\n");
    printf("   (use command \"mutex\")\n");
    printf("   -> Demonstrates mutual exclusion.\n\n");

    printf("3. Semaphore Synchronization\n");
    printf("   (use command \"semaphore\")\n");
    printf("   -> Demonstrates controlled resource access.\n\n");

    printf("4. Back\n");
    printf("   (use command \"back\")\n");

    printf("\nExample:\n");
    printf("SparkOS/Threads> mutex\n");

    while (1)
    {
        printf("\nSparkOS/Threads> ");

        if (fgets(command,
                  sizeof(command),
                  stdin) == NULL)
        {
            return;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "create") == 0)
        {
            threads_demo();
        }
        else if (strcmp(command, "mutex") == 0)
        {
            mutex_demo();
        }
        else if (strcmp(command, "semaphore") == 0)
        {
            semaphore_demo();
        }
        else if (strcmp(command, "back") == 0)
        {
            return;
        }
        else if (strcmp(command, "help") == 0)
        {
            printf("\nAvailable Thread commands:\n");
            printf("create     -> Thread Creation\n");
            printf("mutex      -> Mutex\n");
            printf("semaphore  -> Semaphore\n");
            printf("back       -> Main Shell\n");
        }
        else if (strcmp(command, "clear") == 0)
        {
            clear_screen();
        }
        else if (strlen(command) == 0)
        {
            continue;
        }
        else
        {
            printf("\nUnknown command: %s\n",
                   command);

            printf("Type the command shown beside the topic.\n");
        }
    }
}

/* ============================================================
 *                       MAIN SHELL
 * ============================================================ */

void shell()
{
    char command[COMMAND_SIZE];

    printf("\n");
    printf("============================================================\n");
    printf("                       SPARK OS\n");
    printf("             OS Concepts Learning Environment\n");
    printf("============================================================\n");

    printf("\nWelcome to Spark OS!\n\n");

    printf("Learn operating system concepts through\n");
    printf("theory, diagrams and practical demonstrations.\n");

    printf("\nIMPORTANT:\n");
    printf("The numbers below are ONLY labels.\n");
    printf("Do NOT enter the numbers.\n");
    printf("Type the command written in brackets.\n");

    printf("\nMAIN TOPICS\n");
    print_line();

    printf("\n1. Process Management\n");
    printf("   (use command \"process\")\n");
    printf("   -> Processes, states, scheduling and creation\n");

    printf("\n2. Inter-Process Communication\n");
    printf("   (use command \"ipc\")\n");
    printf("   -> Pipe, FIFO and signals\n");

    printf("\n3. Memory Management\n");
    printf("   (use command \"memory\")\n");
    printf("   -> Memory layout and dynamic allocation\n");

    printf("\n4. File Management\n");
    printf("   (use command \"files\")\n");
    printf("   -> File I/O, descriptors and inode\n");

    printf("\n5. Threads & Synchronization\n");
    printf("   (use command \"threads\")\n");
    printf("   -> Threads, mutex and semaphore\n");

    printf("\n6. System Calls\n");
    printf("   (use command \"syscall\")\n");
    printf("   -> Program-to-OS service requests\n");

    printf("\n7. About Spark OS\n");
    printf("   (use command \"about\")\n");

    printf("\n8. Help\n");
    printf("   (use command \"help\")\n");

    printf("\n9. Exit\n");
    printf("   (use command \"exit\")\n");


    while (1)
    {
        printf("\nSparkOS> ");

        if (fgets(command,
                  sizeof(command),
                  stdin) == NULL)
        {
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "process") == 0)
        {
            process_module();
        }

        else if (strcmp(command, "ipc") == 0)
        {
            ipc_module();
        }

        else if (strcmp(command, "memory") == 0)
        {
            memory_module();
        }

        else if (strcmp(command, "files") == 0)
        {
            files_module();
        }

        else if (strcmp(command, "threads") == 0)
        {
            threads_module();
        }

        else if (strcmp(command, "syscall") == 0)
        {
            syscall_demo();
        }

        else if (strcmp(command, "about") == 0)
        {
            about_sparkos();
        }

        else if (strcmp(command, "help") == 0)
        {
            show_help();
        }

        else if (strcmp(command, "clear") == 0)
        {
            clear_screen();
        }

        else if (strcmp(command, "exit") == 0 ||
                 strcmp(command, "quit") == 0)
        {
            printf("\nExiting Spark OS...\n");
            printf("Thank you for using Spark OS.\n");
            break;
        }

        /* Direct commands */

        else if (strcmp(command, "ps") == 0)
        {
            process_table_demo();
        }

        else if (strcmp(command, "states") == 0)
        {
            process_states_demo();
        }

        else if (strcmp(command, "schedule") == 0)
        {
            scheduler_demo();
        }

        else if (strcmp(command, "create") == 0)
        {
            fork_demo();
        }

        else if (strcmp(command, "pipe") == 0)
        {
            pipe_demo();
        }

        else if (strcmp(command, "fifo") == 0)
        {
            fifo_demo();
        }

        else if (strcmp(command, "signal") == 0)
        {
            signal_demo();
        }

        else if (strcmp(command, "layout") == 0)
        {
            memory_layout_demo();
        }

        else if (strcmp(command, "malloc") == 0)
        {
            malloc_demo();
        }

        else if (strcmp(command, "io") == 0)
        {
            files_demo();
        }

        else if (strcmp(command, "fd") == 0)
        {
            fd_demo();
        }

        else if (strcmp(command, "inode") == 0)
        {
            inode_demo();
        }

        else if (strcmp(command, "thread") == 0)
        {
            threads_demo();
        }

        else if (strcmp(command, "mutex") == 0)
        {
            mutex_demo();
        }

        else if (strcmp(command, "semaphore") == 0)
        {
            semaphore_demo();
        }

        else if (strncmp(command,
                         "echo ",
                         5) == 0)
        {
            printf("%s\n",
                   command + 5);
        }

        else if (strcmp(command, "echo") == 0)
        {
            printf("Usage: echo <text>\n");
        }

        else if (strlen(command) == 0)
        {
            continue;
        }

        else
        {
            printf("\nCommand not found: %s\n",
                   command);

            printf("Type 'help' to see available commands.\n");
        }
    }
}

/* ============================================================
 *                           MAIN
 * ============================================================ */

int main()
{
    process_init();

    shell();

    return 0;
}
