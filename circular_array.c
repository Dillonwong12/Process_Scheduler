#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "circular_array.h"

struct circular_array {
    int head;
    int tail;
    int capacity;
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

    return circularArray;
}

void enqueue(struct circular_array *circularArray, process_t *process){
    if (circularArray->head == INIT_IDX && circularArray->tail == INIT_IDX){
        // This is the first element to be inserted
        circularArray->head = 0;
        circularArray->tail = 0;
        circularArray->processes[circularArray->tail] = *process;
        return;
    }
    else if (((circularArray->tail + 1)%circularArray->capacity) == circularArray->head){
        // Queue is full, reallocate memory
        int prev_size = circularArray->capacity;
        circularArray->capacity *= REALLOC_SCALE;
        circularArray->processes = (process_t*)realloc(circularArray->processes, sizeof(process_t)*circularArray->capacity);
        assert(circularArray->processes);

        // If the head was in front of the tail, shift all the processes from before the head to after it
        if (circularArray->head > circularArray->tail){
            circularArray->tail = circularArray->tail + prev_size;
            for (int i = 0; i < circularArray->head; i++){
                circularArray->processes[i+prev_size] = circularArray->processes[i];
            }
        }   
    }
    circularArray->tail = (circularArray->tail + 1)%circularArray->capacity;
    circularArray->processes[circularArray->tail] = *process;
}

int processes_size(struct circular_array *circularArray){
    if (circularArray->head <= circularArray->tail){
        return circularArray->tail + 1 - circularArray->head; 
    }
    return circularArray->tail + 1 + circularArray->capacity - circularArray->head;
}

void print_array(struct circular_array *circularArray){
    if (circularArray->head <= circularArray->tail){
        for (int i = circularArray->head; i <= circularArray->tail; i++){
            printf("process %d: ", i);
            printf("%u %s %u %d\n", circularArray->processes[i].time_arr, circularArray->processes[i].name, circularArray->processes[i].serv_time, circularArray->processes[i].mem_req);
        }
    }
    else {
        for (int i = circularArray->head; i < circularArray->capacity; i++){
            printf("%u %s %u %d\n", circularArray->processes[i].time_arr, circularArray->processes[i].name, circularArray->processes[i].serv_time, circularArray->processes[i].mem_req);
        }
        for (int i = 0; i <= circularArray->tail; i++){
            printf("%u %s %u %d\n", circularArray->processes[i].time_arr, circularArray->processes[i].name, circularArray->processes[i].serv_time, circularArray->processes[i].mem_req);
        }
    }
}

void free_array(struct circular_array *circularArray){
    free(circularArray->processes);
}