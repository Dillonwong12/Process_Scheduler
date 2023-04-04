#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>

// The number of expected command line arguments
#define N_ARGC 9
// The scale by which the `processes` array is dynamically reallocated 
#define REALLOC_SCALE 2
// The number of attributes per process
#define N_ATTRIBUTES 4
// The maximum length of process names
#define MAX_NAME_LEN 9
// The maximum length of a line from an input file
#define MAX_LINE_LEN 35

// Each process has an arrival time, a unique name, a service time, and a memory requirement
typedef struct process{
    int time_arr;
    char name[MAX_NAME_LEN];
    int serv_time;
    int mem_req;
}process_t;


int main(int argc, char* argv[]){
    int opt;
    char *filename;
    char *scheduler;
    char *memory_strategy;
    int quantum;

    size_t n_processes = 0;
    size_t processes_size = 5;

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
    
    process_t *processes = (process_t*)malloc(sizeof(process_t) * processes_size);
    assert(processes != NULL);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LEN];
    int time_arr, serv_time, mem_req;
    char name[MAX_NAME_LEN];

    while (fgets(line, MAX_LINE_LEN, fp)){
        sscanf(line, "%d %s %d %d", &time_arr, name, &serv_time, &mem_req);
        if (n_processes == processes_size){
            processes_size *= REALLOC_SCALE;
            processes = (process_t*)realloc(processes, sizeof(process_t) * processes_size);
            assert(processes);
        }
        printf("%zu\n", n_processes);
        processes[n_processes].time_arr = time_arr;
        strcpy(processes[n_processes].name, name);
        processes[n_processes].serv_time = serv_time;
        processes[n_processes].mem_req = mem_req;
        n_processes++;
    }

    for (int i = 0; i < n_processes; i++){
        printf("%d %s %d %d\n", processes[i].time_arr, processes[i].name, processes[i].serv_time, processes[i].mem_req);
    } 

    free(processes);
    fclose(fp);
    return 0;
}