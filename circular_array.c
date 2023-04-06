#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "circular_array.h"

struct circular_array {
    int head;
    int tail;
    int capacity;
    int size;
    process_t *processes;
};

struct circular_array *new_circular_array(){
    struct circular_array *circularArray = (struct circular_array*)malloc(sizeof(struct circular_array));
    assert(circularArray);
    circularArray->processes = (process_t*)malloc(sizeof(process_t) * INIT_SIZE);
    assert(circularArray->processes);
    circularArray->head = INIT_IDX;
    circularArray->tail = INIT_IDX;
    circularArray->capacity = INIT_SIZE;
    circularArray->size = 0;

    return circularArray;
}

void enqueue(struct circular_array *circularArray, process_t *process){
    // Queue is full, reallocate memory
    if ((circularArray->size > 0) && ((circularArray->tail)%circularArray->capacity) == circularArray->head){
        int prev_size = circularArray->capacity;
        circularArray->capacity *= REALLOC_SCALE;
        circularArray->processes = (process_t*)realloc(circularArray->processes, sizeof(process_t)*circularArray->capacity);
        assert(circularArray->processes);

        // If the head was in front of the tail, shift all the processes from before the head to after it
        if (circularArray->head >= circularArray->tail){
            circularArray->tail = circularArray->tail + prev_size;
            for (int i = 0; i < circularArray->head; i++){
                circularArray->processes[prev_size+i] = circularArray->processes[i];
            }
        }   
    }
    circularArray->processes[circularArray->tail] = *process;
    circularArray->tail = (circularArray->tail + 1)%circularArray->capacity;
    circularArray->size++;
}

process_t *dequeue(struct circular_array *circularArray){
    if (processes_size(circularArray) == 0){
        return 0;
    }
    else {
        circularArray->size--;
        process_t *process = &(circularArray->processes[circularArray->head]);
        circularArray->head = (circularArray->head + 1)%circularArray->size;
        return process;
    }
}

int processes_size(struct circular_array *circularArray){
    return circularArray->size;
}

void print_array(struct circular_array *circularArray){
    if (circularArray->head < circularArray->tail){
        for (int i = circularArray->head; i < circularArray->tail; i++){
            printf("process %d: ", i);
            printf("%u %s %u %d\n", circularArray->processes[i].time_arr, circularArray->processes[i].name, circularArray->processes[i].serv_time, circularArray->processes[i].mem_req);
        }
    }
    else {
        for (int i = circularArray->head; i < circularArray->capacity; i++){
            printf("%u %s %u %d\n", circularArray->processes[i].time_arr, circularArray->processes[i].name, circularArray->processes[i].serv_time, circularArray->processes[i].mem_req);
        }
        for (int i = 0; i < circularArray->tail; i++){
            printf("%u %s %u %d\n", circularArray->processes[i].time_arr, circularArray->processes[i].name, circularArray->processes[i].serv_time, circularArray->processes[i].mem_req);
        }
    }
}

void free_array(struct circular_array *circularArray){
    free(circularArray->processes);
}