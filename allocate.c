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

void manage_processes(FILE *fp, char *scheduler, char *memory_strategy, int quantum, int *simulation_time);
void schedule(struct circular_array *ready_queue, char *scheduler, process_t **running_process);

int main(int argc, char* argv[]){
    int opt;
    char *filename;
    char *scheduler;
    char *memory_strategy;
    int quantum;
    int simulation_time = 0;

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

    manage_processes(fp, scheduler, memory_strategy, quantum, &simulation_time);
    
    fclose(fp);
    return 0;
}

void manage_processes(FILE *fp, char *scheduler, char *memory_strategy, int quantum, int *simulation_time){
    
    int curr_process = 0;
    long last_fp = 0;
    char line[MAX_LINE_LEN];
    unsigned int time_arr, serv_time;
    char name[MAX_NAME_LEN];
    int mem_req;
    int end = 0;
    process_t *running_process = NULL;
    
        struct circular_array *input_queue = new_circular_array();
        struct circular_array *ready_queue = new_circular_array();

        while (1){
            while (fgets(line, MAX_LINE_LEN, fp)){
                sscanf(line, "%u %s %u %d", &time_arr, name, &serv_time, &mem_req);
                if (time_arr <= *simulation_time){
                    process_t process;
                    process.time_arr = time_arr;
                    strcpy(process.name, name);
                    process.serv_time = serv_time;
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
            
            schedule(ready_queue, scheduler, &running_process);
            
            if (running_process != NULL){
                
                //printf("process %s running, %u remaining \n", running_process->name, running_process->serv_time);
                running_process->serv_time -= quantum;
                if (running_process->serv_time <= 0){
                    //printf("process %s completed\n", running_process->name);
                    // This process has been completed
                    
                    running_process = NULL;
                }
            }
            //printf("simulation_time: %d, head %d\n", *simulation_time, ready_queue->head);
            *simulation_time += quantum;
            if ((running_process == NULL && input_queue->size == 0 && ready_queue->size == 0)){
                //There are no more processes in the input queue, and no READY or RUNNING processes.
                break;
            }
        }
        free_array(input_queue);
        free_array(ready_queue);
}

void schedule(struct circular_array *ready_queue, char *scheduler, process_t **running_process){
    if (*running_process == NULL && ready_queue->size > 0){
        // Make a temporary array to use with qsort()
        process_t *temp_array = (process_t*)malloc(sizeof(process_t)*ready_queue->size);
        for (int i = 0; i < ready_queue->size; i++){
            temp_array[i].time_arr = get_process(ready_queue, i)->time_arr;
            strcpy(temp_array[i].name, get_process(ready_queue, i)->name);
            temp_array[i].serv_time = get_process(ready_queue, i)->serv_time;
            temp_array[i].mem_req = get_process(ready_queue, i)->mem_req;
        }
        qsort(temp_array, ready_queue->size, sizeof(process_t), qsort_comparator);
        char *process_to_schedule = temp_array[0].name;
        

        for (int i = 0; i < ready_queue->size; i++){
            if (strcmp(get_process(ready_queue, i)->name, process_to_schedule) == 0){
                *running_process = remove_process(ready_queue, i);
                free(temp_array);
                return;
            }
        }
        /*int chosen_process_idx = 0;
        unsigned int min_serv_time = get_process(ready_queue, chosen_process_idx)->serv_time;
        for (int i = 1; i < ready_queue->size; i++){
            printf("looking at min-serv-time %u\n", get_process(ready_queue, i)->serv_time);
            if (get_process(ready_queue, i)->serv_time < min_serv_time){
                
                min_serv_time = get_process(ready_queue, i)->serv_time;
                chosen_process_idx = i;
                printf("set min-serv-time %u : process at index %d\n", min_serv_time, chosen_process_idx);
            }
        }
        printf("set running process\n"); 
        *running_process = remove_process(ready_queue, chosen_process_idx);*/
    }
    return;
}