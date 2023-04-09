#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "circular_array.h"

#ifndef __MEMORY_H__
#define __MEMORY_H__

// The static size of the `memory` array
#define MEM_SIZE 2048
// Represents an allocation unit that is currently not allocated to a process
#define FREE '0'
// Represents an allocation unit that is currently allocated to a process
#define FULL '1'
// Used to keep track of processes that are not allocated to memory yet
#define INIT_ADDR -1

typedef struct memory{
    char mem_array[MEM_SIZE];
} memory_t;

memory_t *new_mem_array();

int allocate_best_fit(memory_t *memory, process_t *process);

void deallocate(memory_t *memory, process_t *process);

#endif