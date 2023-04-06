// The initial size of the `processes` array
#define INIT_SIZE 5
// The scale by which the `processes` array is dynamically reallocated 
#define REALLOC_SCALE 2
// The maximum length of process names
#define MAX_NAME_LEN 9
// The initial indexes of `head` and `tail`
#define INIT_IDX -1

struct circular_array;

// Each process has an arrival time, a unique name, a service time, and a memory requirement
typedef struct process{
    unsigned int time_arr;
    char name[MAX_NAME_LEN];
    unsigned int serv_time;
    int mem_req;
}process_t;

struct circular_array *new_circular_array();

process_t *new_process(unsigned int time_arr, char *name, unsigned int serv_time, int mem_req);

void enqueue(struct circular_array *circularArray, process_t *process);

int processes_size(struct circular_array *circularArray);

void print_array(struct circular_array *circularArray);

void free_array(struct circular_array *circularArray);