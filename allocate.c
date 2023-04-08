#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>
#include <math.h>
#include "circular_array.h"

// The number of expected command line arguments
#define N_ARGC 9
// The number of attributes per process
#define N_ATTRIBUTES 4
// The maximum length of a line from an input file
#define MAX_LINE_LEN 35

// Scheduling Algorithms
#define SJF = "SJF"
#define RR = "RR"

// Memory strategies
#define INF "infinite"
#define BF "best-fit"

void manage_processes(FILE *fp, char *scheduler, char *memory_strategy, int quantum);
void schedule(struct circular_array *ready_queue, char *scheduler, process_t **running_process, long simulation_time);
void print_performance_stats(long total_turnaround, int num_processes, double total_overhead, double max_overhead, long simulation_time);

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

void manage_processes(FILE *fp, char *scheduler, char *memory_strategy, int quantum){
    
    long last_fp = 0;
    char line[MAX_LINE_LEN];
    unsigned int time_arr, serv_time;
    char name[MAX_NAME_LEN];
    int mem_req;
    process_t *running_process = NULL;

    // Variables for calculating performance statistics
    long simulation_time = 0;
    long total_turnaround = 0;
    int num_processes = 0;
    double total_overhead = 0;
    double max_overhead = 0;
    
    struct circular_array *input_queue = new_circular_array();
    struct circular_array *ready_queue = new_circular_array();

    while (1){
        if (running_process != NULL){
            running_process->serv_time_remaining -= quantum;
            if (running_process->serv_time_remaining <= 0){
                // This process has been completed
                printf("%ld,FINISHED,process_name=%s,proc_remaining=%d\n", simulation_time, running_process->name, input_queue->size+ready_queue->size);
                
                unsigned int turnaround_time = simulation_time - running_process->time_arr;
                total_turnaround += turnaround_time;
                
                float overhead_time = turnaround_time/(double)running_process->serv_time;
                total_overhead += overhead_time;
                if (overhead_time > max_overhead){
                    max_overhead = overhead_time;
                }

                num_processes++;
                running_process = NULL;
            }
        }

        while (fgets(line, MAX_LINE_LEN, fp)){
            sscanf(line, "%u %s %u %d", &time_arr, name, &serv_time, &mem_req);
            if (time_arr <= simulation_time){
                process_t process;
                process.time_arr = time_arr;
                strcpy(process.name, name);
                process.serv_time = serv_time;
                process.serv_time_remaining = serv_time;
                process.mem_req = mem_req;
                enqueue(input_queue, &process);
            }
            else {
                fseek(fp, last_fp, SEEK_SET);
                break;
            }
            last_fp = ftell(fp);
        }

        if (strcmp(memory_strategy, INF) == 0){
            // There is infinite memory, all arrived processes automatically enter the READY state
            for (int i = 0; i < input_queue->size; i++){
                process_t *ready_process = dequeue(input_queue);
                enqueue(ready_queue, ready_process);
            }
        }
        
        schedule(ready_queue, scheduler, &running_process, simulation_time);
        
        //printf("simulation_time: %ld, head %d\n", *simulation_time, ready_queue->head);
        
        if (feof(fp) && running_process == NULL && input_queue->size == 0 && ready_queue->size == 0){
            //There are no more processes in the input queue, and no READY or RUNNING processes.
            print_performance_stats(total_turnaround, num_processes, total_overhead, max_overhead, simulation_time);
            break;
        }
        simulation_time += quantum;
    }
    free_array(input_queue);
    free_array(ready_queue);
}

void schedule(struct circular_array *ready_queue, char *scheduler, process_t **running_process, long simulation_time){
    if (*running_process == NULL && ready_queue->size > 0){
        // Make a temporary array to use with qsort()
        process_t *temp_array = (process_t*)malloc(sizeof(process_t)*ready_queue->size);
        for (int i = 0; i < ready_queue->size; i++){
            temp_array[i].time_arr = get_process(ready_queue, i)->time_arr;
            strcpy(temp_array[i].name, get_process(ready_queue, i)->name);
            temp_array[i].serv_time = get_process(ready_queue, i)->serv_time;
            temp_array[i].serv_time_remaining = get_process(ready_queue, i)->serv_time_remaining;
            temp_array[i].mem_req = get_process(ready_queue, i)->mem_req;
        }

        qsort(temp_array, ready_queue->size, sizeof(process_t), qsort_comparator);
        char *process_to_schedule = temp_array[0].name;

        for (int i = 0; i < ready_queue->size; i++){
            if (strcmp(get_process(ready_queue, i)->name, process_to_schedule) == 0){
                *running_process = remove_process(ready_queue, i);
                printf("%ld,RUNNING,process_name=%s,remaining_time=%u\n", simulation_time, (*running_process)->name, (*running_process)->serv_time_remaining);

                free(temp_array);
                return;
            }
        }
    }
    return;
}

void print_performance_stats(long total_turnaround, int num_processes, double total_overhead, double max_overhead, long simulation_time){
    
    printf("Turnaround time %.0f\n", ceil(total_turnaround/(double)num_processes));
    printf("Time overhead %.2f %.2f\n", max_overhead, total_overhead/num_processes);
    printf("Makespan %ld\n", simulation_time);
}