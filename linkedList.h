#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#define READY 0
#define RUNNING 1
#define FINISHED 2
#define MAX_LEN 8
#define NUM_FRAMES 512

// data definitions

typedef struct node node_t;

struct node {

    char pid[MAX_LEN+1]; 
    int arr_time;   // arrive time of process
    int remain_time; // remaining exacute time of process
    int service_time; // total service time of process
    int start_time; // start time of process that first running
    int finish_time; // finish time of process
    int memory;     // memory that process needed
    int state;      // current state for process
    node_t* next;
    int address_space; //record the start of address space of one process[0...2048) -> use in first fit
    int frames[NUM_FRAMES]; // frame array, record the frame space is occupied or not, it records 0 or 1
    int num_pages;   // number of frames that current process been allocated
};

typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

// create and return an empty linked list 
list_t *make_empty_list(void);
// insert the node(process) at the end of the queue
list_t *insert_at_foot(list_t *list, node_t* new);
// create the new process
node_t* create_node(char* pid, int arr_time, int remain_time, int memory);
// get the head of the queue and remove it from the queue
node_t* get_head(list_t* list);
// check list is empty or not
int is_empty_list(list_t *list);
// read input from file and put it into input_queue
void read_input(list_t* input_queue, char* filename);
// from input queue get all processes that ready to run on CPU
// store them into ready queue
void get_arr_list(list_t* input_queue, list_t* ready_queue, int time);
// delete node from unknow positio
void delete_node(list_t* list, node_t* node);
// free the list
void free_list(list_t *list);
// count how many process remian in the ready queue
int count_processes(list_t *queue);


#endif

