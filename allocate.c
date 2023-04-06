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
// The initial size of the `processes` array
#define INIT_SIZE 5
// The scale by which the `processes` array is dynamically reallocated 
#define REALLOC_SCALE 2
// The number of attributes per process
#define N_ATTRIBUTES 4
// The maximum length of process names
#define MAX_NAME_LEN 9
// The maximum length of a line from an input file
#define MAX_LINE_LEN 35

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

    printf("filename: %s\n", filename);
    printf("scheduler: %s\n", scheduler);
    printf("memory-strategy: %s\n", memory_strategy);
    printf("quantum: %d\n", quantum);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LEN];
    unsigned int time_arr, serv_time;
    char name[MAX_NAME_LEN];
    int mem_req;

    /*while (fgets(line, MAX_LINE_LEN, fp)){
        sscanf(line, "%u %s %u %d", &time_arr, name, &serv_time, &mem_req);
        if (n_processes == processes_size){
            processes_size *= REALLOC_SCALE;
            processes = (process_t*)realloc(processes, sizeof(process_t) * processes_size);
            assert(processes);
        }
        processes[n_processes].time_arr = time_arr;
        strcpy(processes[n_processes].name, name);
        processes[n_processes].serv_time = serv_time;
        processes[n_processes].mem_req = mem_req;
        n_processes++;
    }*/

    /*for (int i = 0; i < n_processes; i++){
        printf("%u %s %u %d\n", processes[i].time_arr, processes[i].name, processes[i].serv_time, processes[i].mem_req);
    }*/

    struct circular_array *circularArray = new_circular_array();

    while (fgets(line, MAX_LINE_LEN, fp)){
        sscanf(line, "%u %s %u %d", &time_arr, name, &serv_time, &mem_req);
        process_t process;
        process.time_arr = time_arr;
        strcpy(process.name, name);
        process.serv_time = serv_time;
        process.mem_req = mem_req;
        enqueue(circularArray, &process);
    }
    printf("total processes: %d\n", processes_size(circularArray));

    print_array(circularArray);
    dequeue(circularArray);
    dequeue(circularArray);
    dequeue(circularArray);
    print_array(circularArray);
    free_array(circularArray);
    fclose(fp);
    return 0;
}

