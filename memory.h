#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "circular_array.h"

#ifndef __MEMORY_H__
#define __MEMORY_H__

// The static size of the memory being simulated
#define MEM_SIZE 2048
// Represents an allocation unit that is currently not allocated to a process
#define FREE '0'
// Represents an allocation unit that is currently allocated to a process
#define FULL '1'
// Used to keep track of processes that are not allocated to memory yet
#define INIT_ADDR -1

// Uses a dynamic array `mem_array` to represent memory allocation units
typedef struct memory{
    char mem_array[MEM_SIZE];
} memory_t;

// Allocates memory for a new dynamic array, and returns a pointer to it
memory_t *new_mem_array();

// Implementation of the Best Fit memory allocation algorithm, for simulating memory allocation for processes in the 
// `input` queue before moving them to the `ready` queue
int allocate_best_fit(memory_t *memory, process_t *process);

// Simulates memory deallocation by updating the memory addresses previously allocated to a `process` to `FREE`
void deallocate(memory_t *memory, process_t *process);

#endif