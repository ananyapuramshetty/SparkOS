#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 5

/*
 * Process states
 */
typedef enum
{
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} process_state;


/*
 * Process Control Block
 */
typedef struct
{
    int pid;
    char name[32];
    process_state state;
    int priority;
    int burst_time;
} PCB;


/*
 * Process management functions
 */
void process_init(void);
void process_list(void);
void process_show_states(void);

#endif
