#include "memory.h"

memory_t *new_mem_array(){
    memory_t *memory = (memory_t *)malloc(sizeof(memory_t));
    memset(memory->mem_array, FREE, MEM_SIZE);
    assert(memory);
    return memory;
}

int allocate_best_fit(memory_t *memory, process_t *process){
    int address = 0;
    int mem_req = process->mem_req;
    int contiguous_units = 0;
    int best_fit_size = MEM_SIZE;
    int best_fit_address = INIT_ADDR;

    for (int i = 0; i < MEM_SIZE; i++){
        // A `FREE` allocation unit has been found, increment `contiguous_units`. If this is the first `FREE` unit 
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
        // A `FULL` allocation unit has been found or this is the end of `mem_array`, reset the number of `contiguous_units`
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
        /*for (int i = 0; i < MEM_SIZE; i++){
            printf("%c ", memory->mem_array[i]);
        }
        printf("\n");*/
    }

    return best_fit_address;
}

void deallocate(memory_t *memory, process_t *process){
    for (int i = process->mem_addr; i < process->mem_addr+process->mem_req; i++){
        memory->mem_array[i] = FREE;
    }
    process->mem_addr = INIT_ADDR;
}