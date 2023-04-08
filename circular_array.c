#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "circular_array.h"

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
    if (circularArray->size == circularArray->capacity){
        
        int prev_size = circularArray->capacity;
        circularArray->capacity *= REALLOC_SCALE;
        circularArray->processes = (process_t*)realloc(circularArray->processes, sizeof(process_t)*circularArray->capacity);
        assert(circularArray->processes);

        // If the head was in front of the tail, shift all the processes from before the head to after it
        if (circularArray->head >= circularArray->tail){
            for (int i = 0; i < circularArray->tail; i++){
                //printf("writing index %d to index %d\n", i, prev_size+i);
                circularArray->processes[prev_size+i] = circularArray->processes[i];
            }
            circularArray->tail = circularArray->tail + prev_size;
        }   
    }
    circularArray->processes[circularArray->tail] = *process;
    circularArray->tail = (circularArray->tail + 1)%circularArray->capacity;
    circularArray->size++;
}

process_t *dequeue(struct circular_array *circularArray){
    if (circularArray->size == 0){
        return 0;
    }
    else {
        process_t *copy = (process_t *)malloc(sizeof(process_t));
        copy->time_arr = circularArray->processes[circularArray->head].time_arr;
        strcpy(copy->name, circularArray->processes[circularArray->head].name);
        copy->serv_time = circularArray->processes[circularArray->head].serv_time;
        copy->serv_time_remaining = circularArray->processes[circularArray->head].serv_time_remaining;
        copy->mem_req = circularArray->processes[circularArray->head].mem_req;
        circularArray->size--;
        circularArray->head = (circularArray->head + 1)%circularArray->capacity;
        return copy;
    }
}

process_t *get_process(struct circular_array *circularArray, int index){
    return &circularArray->processes[(circularArray->head+index)%circularArray->capacity];
}

process_t *remove_process(struct circular_array *circularArray, int index){
    if (circularArray->size == 0){
        return 0;
    }
    else {
        //printf("removing process at index %d + %d = %d\n", circularArray->head, index, (circularArray->head+index)%circularArray->capacity);
        
        for (int i = index; i > 0; i--){
            
            process_t tmp = circularArray->processes[(circularArray->head+i)%circularArray->capacity];
            circularArray->processes[(circularArray->head+i)%circularArray->capacity] = circularArray->processes[(circularArray->head+i-1)%circularArray->capacity];
            circularArray->processes[(circularArray->head+i-1)%circularArray->capacity] = tmp;
        }
        return dequeue(circularArray);
    }
}

int qsort_comparator(const void *process_1, const void *process_2){
    process_t *p1 = (process_t*)process_1;
    process_t *p2 = (process_t*)process_2;
    return p1->serv_time - p2->serv_time;
    if (p1->serv_time == p2->serv_time){
        if (p1->time_arr == p2->time_arr){
            return strcmp(p1->name, p2->name);
        }
        else {
            return p1->time_arr - p2->time_arr;
        }
    }
    else {
        return p1->serv_time - p2->serv_time;
    }
}

void print_array(struct circular_array *circularArray){
    if (circularArray->size == 0){
        return;
    }
    else if (circularArray->head < circularArray->tail){
        for (int i = circularArray->head; i < circularArray->tail; i++){
            printf("index %d: ", i);
            printf("%u %s %ld %d\n", circularArray->processes[i].time_arr, circularArray->processes[i].name, circularArray->processes[i].serv_time, circularArray->processes[i].mem_req);
        }
    }
    else {
        for (int i = circularArray->head; i < circularArray->capacity; i++){
            printf("index %d: %u %s %ld %d\n", i, circularArray->processes[i].time_arr, circularArray->processes[i].name, circularArray->processes[i].serv_time, circularArray->processes[i].mem_req);
        }
        for (int i = 0; i < circularArray->tail; i++){
            printf("index %d: %u %s %ld %d\n", i, circularArray->processes[i].time_arr, circularArray->processes[i].name, circularArray->processes[i].serv_time, circularArray->processes[i].mem_req);
        }
    }
}

void free_array(struct circular_array *circularArray){
    free(circularArray->processes);
}