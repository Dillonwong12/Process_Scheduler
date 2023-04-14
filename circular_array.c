/**
 * Implementation of the circular array data structure. Elements are enqueued to the tail, and dequeued from the head.
*/

#include "circular_array.h"

/**
 * Allocates memory for a new circular array `circ_array`, initialises its attributes, and returns a pointer to it.
*/
circ_array_t *new_circular_array(){
    circ_array_t *circ_array = (circ_array_t*)malloc(sizeof(circ_array_t));
    assert(circ_array);
    circ_array->processes = (process_t**)malloc(sizeof(process_t*) * INIT_SIZE);
    assert(circ_array->processes);
    circ_array->head = INIT_IDX;
    circ_array->tail = INIT_IDX;
    circ_array->capacity = INIT_SIZE;
    circ_array->size = 0;

    return circ_array;
}

/**
 * Enqueues process_t* `process` at the tail of `circ_array->processes`. Also dynamically reallocates memory for 
 * `circ_array->processes` if its size has reached maximum capacity. 
*/
void enqueue(circ_array_t *circ_array, process_t *process){
    // Queue is full, reallocate memory
    if (circ_array->size == circ_array->capacity){
        int prev_size = circ_array->capacity;
        circ_array->capacity *= REALLOC_SCALE;
        circ_array->processes = (process_t**)realloc(circ_array->processes, sizeof(process_t*)*circ_array->capacity);
        assert(circ_array->processes);

        // If the head was in front of the tail, shift all the processes from before the head to after it
        if (circ_array->head >= circ_array->tail){
            for (int i = 0; i < circ_array->tail; i++){
                circ_array->processes[prev_size+i] = circ_array->processes[i];
            }
            circ_array->tail = circ_array->tail + prev_size;
        }   
    }

    // Enqueue `process`
    circ_array->processes[circ_array->tail] = process;
    circ_array->tail = (circ_array->tail+1)%circ_array->capacity;
    circ_array->size++;
}

/**
 * Dequeues a `process` from the head of `circ_array->processes`, returning a pointer to it.
*/
process_t *dequeue(circ_array_t *circ_array){
    if (circ_array->size == 0){
        return 0;
    }
    else {
        process_t *process = circ_array->processes[circ_array->head];
        circ_array->size--;
        circ_array->head = (circ_array->head + 1)%circ_array->capacity;
        return process;
    }
}

// Returns a pointer to a process at an `index` of `circ_array->processes`, starting from the head.
process_t *get_process(circ_array_t *circ_array, int index){
    return circ_array->processes[(circ_array->head+index)%circ_array->capacity];
}

/**
 * Removes a process at an `index` of `circ_array->processes` by swapping elements until it reaches the head
 * and dequeueing. Returns a pointer to the process.
*/
process_t *remove_process(circ_array_t *circ_array, int index){
    if (circ_array->size == 0){
        return 0;
    }
    else {
        // Swap elements until the element which was at `index` reaches the head, then dequeue it
        for (int i = index; i > 0; i--){
            process_t *tmp = circ_array->processes[(circ_array->head+i)%circ_array->capacity];
            circ_array->processes[(circ_array->head+i)%circ_array->capacity] = circ_array->processes[(circ_array->head+i-1)%circ_array->capacity];
            circ_array->processes[(circ_array->head+i-1)%circ_array->capacity] = tmp;
        }
        return dequeue(circ_array);
    }
}

/**
 * Used with qsort() in the Shortest Job First scheduling algorithm to sort processes in ascending order, by 
 * service time, arrival time, and name. 
*/
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

/**
 * Prints out all elements contained in a `circ_array`, starting from the head.
*/
void print_array(circ_array_t *circ_array){
    if (circ_array->size == 0){
        return;
    }
    else if (circ_array->head < circ_array->tail){
        for (int i = circ_array->head; i < circ_array->tail; i++){
            printf("index %d: %u %s %ld %d\n", i, circ_array->processes[i]->time_arr, circ_array->processes[i]->name, circ_array->processes[i]->serv_time, circ_array->processes[i]->mem_req);
        }
    }
    else {
        for (int i = circ_array->head; i < circ_array->capacity; i++){
            printf("index %d: %u %s %ld %d\n", i, circ_array->processes[i]->time_arr, circ_array->processes[i]->name, circ_array->processes[i]->serv_time, circ_array->processes[i]->mem_req);
        }
        for (int i = 0; i < circ_array->tail; i++){
            printf("index %d: %u %s %ld %d\n", i, circ_array->processes[i]->time_arr, circ_array->processes[i]->name, circ_array->processes[i]->serv_time, circ_array->processes[i]->mem_req);
        }
    }
}

// Frees all memory used by `circ_array`.
void free_array(circ_array_t *circ_array){
    free(circ_array->processes);
    free(circ_array);
}