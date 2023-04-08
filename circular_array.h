// The initial size of the `processes` array
#define INIT_SIZE 5
// The scale by which the `processes` array is dynamically reallocated 
#define REALLOC_SCALE 2
// The maximum length of process names
#define MAX_NAME_LEN 9
// The initial indexes of `head` and `tail`
#define INIT_IDX 0 

// Each process has an arrival time, a unique name, a service time, and a memory requirement
typedef struct process{
    unsigned int time_arr;
    char name[MAX_NAME_LEN];
    long serv_time;
    int mem_req;
    long serv_time_remaining;
}process_t;

struct circular_array {
    int head;
    int tail;
    int capacity;
    int size;
    process_t *processes;
};

struct circular_array *new_circular_array();

process_t *new_process(unsigned int time_arr, char *name, unsigned int serv_time, int mem_req);

void enqueue(struct circular_array *circularArray, process_t *process);

process_t *dequeue(struct circular_array *circularArray);

process_t *get_process(struct circular_array *circularArray, int index);

process_t *remove_process(struct circular_array *circularArray, int index);

int qsort_comparator(const void *process_1, const void *process_2);

void print_array(struct circular_array *circularArray);

void free_array(struct circular_array *circularArray);