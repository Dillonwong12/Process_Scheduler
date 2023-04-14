#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef __CIRCULAR_ARRAY_H__
#define __CIRCULAR_ARRAY_H__

// The initial size of each `processes` array
#define INIT_SIZE 5
// The scale by which the `processes` array is dynamically reallocated 
#define REALLOC_SCALE 2
// The maximum length of process names
#define MAX_NAME_LEN 9
// The initial indexes of `head` and `tail`
#define INIT_IDX 0 

/**
 * Each process has an arrival time, a unique name, a service time, and a memory requirement. `serv_time_remaining` is
 * used to keep track of the remaining service time of a process after each cycle of the process manager, and 
 * `mem_addr` is used to keep track of the memory address that the allocation for a process starts at. 
*/ 
typedef struct process{
    unsigned int time_arr;
    char name[MAX_NAME_LEN];
    long serv_time;
    int mem_req;
    long serv_time_remaining;
    int mem_addr;
} process_t;

/**
 * Processes are enqueued to the tail of the `processes` array, and dequeued from the head. `capacity` is the maximum
 * capacity of the `processes` array, and `size` is the current number of processes contained in the queue.
*/
typedef struct circular_array{
    int head;
    int tail;
    int capacity;
    int size;
    process_t **processes;
} circ_array_t;

// Allocates memory for a new circular array `circ_array`, initialises its attributes, and returns a pointer to it.
circ_array_t *new_circular_array();

// Enqueues process_t* `process` at the tail of `circ_array->processes`, dynamically reallocating memory if 
// `circ_array->processes` has reached maximum capacity.
void enqueue(circ_array_t *circ_array, process_t *process);

// Dequeues a `process` from the head of `circ_array->processes`, returning a pointer to it.
process_t *dequeue(circ_array_t *circ_array);

// Returns a pointer to a process at an `index` of `circ_array->processes`, starting from the head.
process_t *get_process(circ_array_t *circ_array, int index);

// Removes a `process` at an `index` of `circ_array->processes`. Returns a pointer to the `process`.
process_t *remove_process(circ_array_t *circ_array, int index);

// Used with qsort() in the SJF scheduling algorithm to sort processes by service time, arrival time, and name. 
int qsort_comparator(const void *process_1, const void *process_2);

// Prints out all elements contained in a `circ_array`, starting from the head.
void print_array(circ_array_t *circ_array);

// Frees all memory used by `circ_array`.
void free_array(circ_array_t *circ_array);

#endif