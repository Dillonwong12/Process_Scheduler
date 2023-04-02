#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// The number of expected command line arguments
int N_ARGC = 9;

int main(int argc, char* argv[]){
    int opt;
    char *filename;
    char *scheduler;
    char *memory_strategy;
    int quantum;

    if (argc != N_ARGC){
        fprintf(stderr, "Error: malformed command line arguments\n");
        exit(-1);
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
            quantum = (int)optarg[0];
        } 
    }

    printf("filename: %s\n", filename);
    printf("scheduler: %s\n", scheduler);
    printf("memory-strategy: %s\n", memory_strategy);
    printf("quantum: %d\n", quantum);
    return 0;
}