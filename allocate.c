#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "circular_array.h"
#include "memory.h"

#define IMPLEMENTS_REAL_PROCESS

// The number of expected command line arguments
#define N_ARGC 9
// The number of bytes after converting an integer value to byte array
#define N_BYTES 4
// The maximum length of a line from an input file
#define MAX_LINE_LEN 35
// Length of the byte string from the standard output of process upon termination
#define SHA_LEN 65
// Used to check if real processes have been created
#define INIT_PID -1

// Scheduling algorithms
#define SJF "SJF"
#define RR "RR"

// Memory strategies
#define INF "infinite"
#define BF "best-fit"

//  Simulates a process manager, which checks for running process completion, then performs process submission, memory 
// allocation, and process scheduling in cycles.
void manage_processes(FILE *fp, char *scheduler, char *memory_strategy, int quantum);

// Simulates memory allocation for processes according to `memory_strategy`. If `memory_strategy` is `INF`, then memory
// is infinite and all processes can be moved from `input_queue` to `ready_queue`. Otherwise, apply the Best Fit memory
// allocation algorithm. 
void allocate_memory(circ_array_t *input_queue, circ_array_t *ready_queue, memory_t *memory, char *memory_strategy, long simulation_time);

// Applies a scheduling algorithm according to `scheduler`, to determine the `running_process` for a cycle. If 
// `scheduler` is `SJF`, apply the Shortest Job First algorithm. Otherwise, apply the Round Robin algorithm.
void schedule(circ_array_t *ready_queue, char *scheduler, process_t **running_process, long simulation_time);

// Prints performance statistics: average turnaround time, time overhead (maximum and average), and makespan.
void print_performance_stats(long total_turnaround, long num_processes, double total_overhead, double max_overhead, long simulation_time);

// Converts a given long `value` to a 32-bit representation in `byte_array` with Big Endian Ordering 
void to_byte_array(long value, uint8_t *byte_array);

// Creates a real process
void create_process(process_t *running_process, long simulation_time);

// Resumes a real process
void resume_process(process_t *running_process, long simulation_time);

// Suspends a real process
void suspend_process(process_t *running_process, long simulation_time);

// Terminates a real process
void terminate_process(process_t *running_process, long simulation_time);

int main(int argc, char* argv[]){
    int opt;
    char *filename;
    char *scheduler;
    char *memory_strategy;
    int quantum;

    if (argc != N_ARGC){
        fprintf(stderr, "Error: malformed command line arguments\n");
        exit(EXIT_FAILURE);
    }

    // Parse command-line arguments
    while ((opt = getopt(argc, argv, ":f:s:m:q:")) != -1){
        if (opt == 'f'){
            filename = optarg;
        }
        else if (opt == 's'){
            scheduler = optarg;
        }
        else if (opt == 'm'){
            memory_strategy = optarg;
        }
        else if (opt == 'q'){
            quantum = atoi(optarg);
        } 
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    manage_processes(fp, scheduler, memory_strategy, quantum);
    
    fclose(fp);
    return 0;
}

/**
 * Simulates a process manager, which runs in cycles. At the beginning of each cycle, checks if the `running_process`
 * has completed execution, and terminates the process if so. Next, checks for processes that have been submitted
 * to the system since the last cycle, where processes are considered to be submitted if their `time_arr` is less than
 * or equal to the current `simulation_time`. Processes that have arrived are inserted into the `input_queue`. Then, 
 * attempts to allocate memory to processes in the `input_queue`. Processes with successful memory allocation are moved
 * to the `ready_queue`. A scheduler (`SJF` or `RR`) will then determine which process gets to run for this cycle.
 * The cycle repeats until no processes remain.
*/
void manage_processes(FILE *fp, char *scheduler, char *memory_strategy, int quantum){
    
    long last_fp = 0;
    char line[MAX_LINE_LEN];
    unsigned int time_arr;
    char name[MAX_NAME_LEN];
    long serv_time;
    int mem_req;
    process_t *running_process = NULL;

    // Variables for calculating performance statistics
    long simulation_time = 0;
    long total_turnaround = 0;
    long num_processes = 0;
    double total_overhead = 0;
    double max_overhead = 0;
    
    circ_array_t *input_queue = new_circular_array();
    circ_array_t *ready_queue = new_circular_array();
    memory_t *memory = new_mem_array();

    while (1){
        if (running_process != NULL){
            running_process->serv_time_remaining -= quantum;
            
            // Check if the currently `running_process` has been completed
            if (running_process->serv_time_remaining <= 0){
                printf("%ld,FINISHED,process_name=%s,proc_remaining=%d\n", simulation_time, running_process->name, input_queue->size+ready_queue->size);
                
                long turnaround_time = simulation_time - running_process->time_arr;
                total_turnaround += turnaround_time;
                
                double overhead_time = turnaround_time/(double)running_process->serv_time;
                total_overhead += overhead_time;
                if (overhead_time > max_overhead){
                    max_overhead = overhead_time;
                }
                num_processes++;

                // If Best Fit memory allocation algorithm is being simulated, deallocate the `memory`
                if (strcmp(memory_strategy, BF) == 0){
                    deallocate(memory, running_process);
                }

                terminate_process(running_process, simulation_time);
                
                free(running_process);
                running_process = NULL;
            }
        }

        // Check if any processes have been submitted to the system, and add them to the `input_queue`
        while (fgets(line, MAX_LINE_LEN, fp)){
            sscanf(line, "%u %s %ld %d", &time_arr, name, &serv_time, &mem_req);
            if (time_arr <= simulation_time){
                process_t *process = malloc(sizeof(process_t));
                assert(process);
                process->time_arr = time_arr;
                strcpy(process->name, name);
                process->serv_time = serv_time;
                process->serv_time_remaining = serv_time;
                process->mem_req = mem_req;
                process->mem_addr = INIT_ADDR;
                process->pid = INIT_PID;
                enqueue(input_queue, process);
            }
            // Reset `fp` to `last_fp` if no more processes are submitted
            else {
                fseek(fp, last_fp, SEEK_SET);
                break;
            }
            last_fp = ftell(fp);
        }

        // Perform memory allocation
        allocate_memory(input_queue, ready_queue, memory, memory_strategy, simulation_time);
        
        // Determine the `running_process` for this cycle
        schedule(ready_queue, scheduler, &running_process, simulation_time);
        if (running_process != NULL){
            // When a scheduling algorithm selects a process to run for the first time, the process is created. 
            // Otherwise, it is resumed.
            if (running_process->pid == INIT_PID){
                create_process(running_process, simulation_time);
            }
            else {
                resume_process(running_process, simulation_time);
            }
        }
        
        // There are no more processes in the input file, `input_queue`, `ready_queue`, and no `running_process`,
        // the simulation is complete!
        if (feof(fp) && running_process == NULL && input_queue->size == 0 && ready_queue->size == 0){
            print_performance_stats(total_turnaround, num_processes, total_overhead, max_overhead, simulation_time);
            break;
        }
        simulation_time += quantum;
    }

    free(memory);
    free_array(input_queue);
    free_array(ready_queue);
}

/**
 * Simulates memory allocation for processes according to `memory_strategy`. If `memory_strategy` is `INF`, then memory
 * is infinite and all processes can be moved from `input_queue` to `ready_queue`. Otherwise, apply the Best Fit memory
 * allocation algorithm.
*/
void allocate_memory(circ_array_t *input_queue, circ_array_t *ready_queue, memory_t *memory, char *memory_strategy, long simulation_time){
    
    // There is infinite memory, all arrived processes automatically enter the READY state
    if (strcmp(memory_strategy, INF) == 0){
        for (int i = 0; i < input_queue->size; i++){
            process_t *ready_process = dequeue(input_queue);
            enqueue(ready_queue, ready_process);
        }
    }
    else {
        // Default to Best-Fit memory allocation algorithm otherwise, iterating over each process in `input_queue`
        for (int i = 0; i < input_queue->size; i++){
            process_t *process = get_process(input_queue, i);

            // Successful memory allocation!
            if (allocate_best_fit(memory, process) != INIT_ADDR){
                printf("%ld,READY,process_name=%s,assigned_at=%d\n", simulation_time, process->name, process->mem_addr);
                process_t *ready_process = remove_process(input_queue, i);
                enqueue(ready_queue, ready_process);
                i--;
            }
        }
    }
}

/**
 * Applies a scheduling algorithm according to `scheduler`, to determine the `running_process` for a cycle. If 
 * `scheduler` is `SJF`, apply the Shortest Job First algorithm. Otherwise, apply the Round Robin algorithm.
*/
void schedule(circ_array_t *ready_queue, char *scheduler, process_t **running_process, long simulation_time){
    
    // Apply the Shortest Job First scheduling algorithm
    if (strcmp(scheduler, SJF) == 0){
        if (*running_process == NULL && ready_queue->size > 0){
            // Make a temporary array to use with qsort()
            process_t *temp_array = (process_t*)malloc(sizeof(process_t)*ready_queue->size);
            for (int i = 0; i < ready_queue->size; i++){
                temp_array[i].time_arr = get_process(ready_queue, i)->time_arr;
                strcpy(temp_array[i].name, get_process(ready_queue, i)->name);
                temp_array[i].serv_time = get_process(ready_queue, i)->serv_time;
                temp_array[i].serv_time_remaining = get_process(ready_queue, i)->serv_time_remaining;
                temp_array[i].mem_req = get_process(ready_queue, i)->mem_req;
                temp_array[i].mem_addr = get_process(ready_queue, i)->mem_addr;
            }

            qsort(temp_array, ready_queue->size, sizeof(process_t), qsort_comparator);
            char *process_to_schedule = temp_array[0].name;

            // Determine the `running_process` by its unique name
            for (int i = 0; i < ready_queue->size; i++){
                if (strcmp(get_process(ready_queue, i)->name, process_to_schedule) == 0){
                    *running_process = remove_process(ready_queue, i);
                    printf("%ld,RUNNING,process_name=%s,remaining_time=%ld\n", simulation_time, (*running_process)->name, (*running_process)->serv_time_remaining);
                    free(temp_array);
                    return;
                }
            }
        }
        
    }
    else {
        // Default to the Round Robin scheduling algorithm if scheduler isn't `SJF`
        // If there are no other READY processes, there are no other processes to schedule, so just return
        if (ready_queue->size == 0){
            return;
        }

        // If there are other READY processes but no current `running_process`, dequeue from `ready_queue`
        if (*running_process == NULL){
            *running_process = dequeue(ready_queue);
            printf("%ld,RUNNING,process_name=%s,remaining_time=%ld\n", simulation_time, (*running_process)->name, (*running_process)->serv_time_remaining);
            return;
        }

        // If there are other READY processes, the current `running_process` enters the `ready_queue`, and another 
        // process is scheduled to run. The real `running_process` is also suspended here.
        suspend_process(*running_process, simulation_time);
        enqueue(ready_queue, *running_process);
        *running_process = dequeue(ready_queue);
        printf("%ld,RUNNING,process_name=%s,remaining_time=%ld\n", simulation_time, (*running_process)->name, (*running_process)->serv_time_remaining);
        
    }
    return;
}

/**
 * Prints performance statistics: average turnaround time, time overhead (maximum and average), and makespan.
*/
void print_performance_stats(long total_turnaround, long num_processes, double total_overhead, double max_overhead, long simulation_time){
    
    printf("Turnaround time %.0f\n", ceil(total_turnaround/(double)num_processes));
    printf("Time overhead %.2f %.2f\n", round(max_overhead*100)/100, round((total_overhead/(double)num_processes)*100)/100);
    printf("Makespan %ld\n", simulation_time);
}

/**
 * Converts a given `value` from long to an array of `bytes` in Big Endian ordering
*/
void to_byte_array(long value, uint8_t *bytes){
    /* Right shift for multiples of 8, and apply the bitwise AND operator to extract each 8-bit chunk of `value`.
    This function was referenced from Lundin. 
    https://stackoverflow.com/questions/22605399/integer-to-byte-array-little-endian-and-vice-versa.
    24/3/2014. Accessed on 14/4/2023. Modified to work with long instead of uint32_t. */  

    bytes[0] = (uint8_t)((value >> 24)&0xFF);
    bytes[1] = (uint8_t)((value >> 16)&0xFF);
    bytes[2] = (uint8_t)((value >> 8)&0xFF);
    bytes[3] = (uint8_t)(value&0xFF);
    return;
}

/**
 * Creates a real process using fork() and execv(). Uses pipe() and dup2() to redirect STDIN and STDOUT to and from 
 * "process". Sends the 32-bit `simulation_time` to STDIN of "process", and reads 1 byte from STDOUT of "process" to
 * make sure that a similar least significant byte was sent.
*/
void create_process(process_t *running_process, long simulation_time){
    char *args[] = {"process", running_process->name, NULL};
    uint8_t read_byte;
    uint8_t byte_array[N_BYTES];
    
    /* Creates two pipes. The idea of using two pipes for two-way communication between parent and child process was 
    referenced from tutorialspoint. Inter Process Communication. 
    https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_pipes.htm.
    Accessed on 15/4/2023, and modified to work with process_t struct for this project. */
    if (pipe(running_process->fd_1) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(running_process->fd_2) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create child process
    pid_t pid = fork();

    if (pid == 0){     
        // Close the write side of `fd_1`, and the read side of `fd_2`
        close(running_process->fd_1[1]);
        close(running_process->fd_2[0]);
        // Redirect STDIN and STDOUT to and from "process"
        dup2(running_process->fd_1[0], STDIN_FILENO);
        dup2(running_process->fd_2[1], STDOUT_FILENO);
        execv("process", args);
        exit(0);
    }
    if (pid > 0){
        running_process->pid = pid;
        // Close the read side of `fd_1`, and the write side of `fd_2`
        close(running_process->fd_1[0]);
        close(running_process->fd_2[1]);
        to_byte_array(simulation_time, byte_array);
        // Write `simulation_time` to "process", and read 1 byte from the STDOUT of "process"
        write(running_process->fd_1[1], byte_array, N_BYTES);
        read(running_process->fd_2[0], &read_byte, sizeof(read_byte));
        if (read_byte != byte_array[N_BYTES-1]){
            perror("incorrect byte");
        }
    }
}

/**
 * Resumes a real process by first sending the `simulation_time` to STDIN of "process", then sending a SIGCONT signal 
 * to "process". Reads 1 byte from STDOUT of "process" to make sure that a similar least significant byte was sent.
*/
void resume_process(process_t *running_process, long simulation_time){
    uint8_t read_byte;
    uint8_t byte_array[N_BYTES];
    to_byte_array(simulation_time, byte_array);

    write(running_process->fd_1[1], byte_array, N_BYTES);
    kill(running_process->pid, SIGCONT);
    read(running_process->fd_2[0], &read_byte, sizeof(read_byte));
    if (read_byte != byte_array[N_BYTES-1]){
        perror("incorrect byte");
    }
}

/**
 * Suspends a real process by first sending the `simulation_time` to STDIN of "process", then sending a SIGTSTP signal 
 * to "process".
*/
void suspend_process(process_t *running_process, long simulation_time){
    uint8_t byte_array[N_BYTES];
    int status;

    to_byte_array(simulation_time, byte_array);
    write(running_process->fd_1[1], byte_array, N_BYTES);
    kill(running_process->pid, SIGTSTP);

    // Wait for the process to enter a stopped state
    do {
        waitpid(running_process->pid, &status, WUNTRACED);
    }
    while (!WIFSTOPPED(status));
}

/**
 * Terminates a real process by first sending the `simulation_time` to STDIN of "process", then sending a SIGTERM signal 
 * to "process". Reads a 64-byte string from STDOUT of "process" to be included in the execution transcript.
*/
void terminate_process(process_t *process, long simulation_time){
    uint8_t byte_array[N_BYTES];
    uint8_t sha_array[SHA_LEN];
    int status;

    to_byte_array(simulation_time, byte_array);
    write(process->fd_1[1], byte_array, N_BYTES);
    // Wait for the child process to be successfully terminated
    if (kill(process->pid, SIGTERM) == 0){
        waitpid(process->pid, &status, 0);
    }
    
    read(process->fd_2[0], sha_array, SHA_LEN);
    sha_array[SHA_LEN-1] = '\0';
    printf("%ld,FINISHED-PROCESS,process_name=%s,sha=%s\n", simulation_time, process->name, sha_array);
}