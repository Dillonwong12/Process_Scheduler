/**
 * Implementation of `memory_t`, a struct containing a dynamic array used to simulate memory allocation.
*/
#include "memory.h"

/**
 * Allocates memory for a new dynamic array `memory`, and returns a pointer to it.
*/
memory_t *new_mem_array(){
    memory_t *memory = (memory_t *)malloc(sizeof(memory_t));
    // All allocation units are initially `FREE`
    memset(memory->mem_array, FREE, MEM_SIZE);
    assert(memory);
    return memory;
}

/**
 * Implementation of the Best Fit memory allocation algorithm, for simulating memory allocation for processes in the 
 * `input` queue before moving them to the `ready` queue. Takes memory_t* `memory`, and process_t* `process`, attempts 
 * to allocate memory according to `process->mem_req` requirements. Returns `best_fit_address`, which will be a 
 * non-negative integer indicating the starting address of the allocated memory if allocation was successful.
*/
int allocate_best_fit(memory_t *memory, process_t *process){
    int address = 0;
    int mem_req = process->mem_req;
    int contiguous_units = 0;
    int best_fit_size = MEM_SIZE;
    int best_fit_address = INIT_ADDR;

    for (int i = 0; i < MEM_SIZE; i++){
        // A `FREE` allocation unit has been found. Increment `contiguous_units`. If this is the first `FREE` unit 
        // in a chunk of memory, take note of the `address` where it starts. If the entire `mem_array` is `FREE`,
        // return `address`.
        if (memory->mem_array[i] == FREE){
            if (contiguous_units == 0){
                address = i;
            }
            else if (contiguous_units == MEM_SIZE-1){
                best_fit_address = address;
            }
            contiguous_units++;
        }
        // A `FULL` allocation unit has been found or this is the end of `mem_array`, check if the number of 
        // `contiguous_units` was sufficient for memory allocation, then reset the number of `contiguous_units`
        if ((memory->mem_array[i] == FULL || i == MEM_SIZE-1) && contiguous_units > 0){
            if (contiguous_units < best_fit_size && contiguous_units >= mem_req){
                best_fit_size = contiguous_units;
                best_fit_address = address;
            }
            contiguous_units = 0;

        }
    }

    // If memory was succesfully allocated to memory, update the addresses in `mem_array` to `FULL`.
    if (best_fit_address != INIT_ADDR){
        process->mem_addr = best_fit_address;
        for (int i = best_fit_address; i < best_fit_address+mem_req; i++){
            memory->mem_array[i] = FULL;
        }
    }

    return best_fit_address;
}

/**
 * Simulates memory deallocation by updating the memory addresses previously allocated to a `process` to `FREE`
 */
void deallocate(memory_t *memory, process_t *process){
    for (int i = process->mem_addr; i < process->mem_addr+process->mem_req; i++){
        memory->mem_array[i] = FREE;
    }
    process->mem_addr = INIT_ADDR;
}