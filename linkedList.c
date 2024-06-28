 // include
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "linkedList.h"


/**
 * some structure and codes for linked list is reference from COMP10002
*/

// function implementation
// make an empty queue
list_t* make_empty_list(void) {
    // malloc some meory to linked list
    list_t* list = (list_t*)malloc(sizeof(list_t));
    assert(list);
    
    // initialize head and foot to NULL
   	list->head = list->foot = NULL;

    return list;
}

// insert the new process at the end of the queue
list_t* insert_at_foot(list_t *list, node_t* new) {
    assert(new && list);

    // if current list is empty

    if (list->head == NULL) {
        list->head = new;
    } else {
        list->foot->next = new;


    }
    list->foot = new;
    return list;
}	

// create process based on input information
node_t* create_node(char* pid, int arr_time, int remain_time, int memory) {
    node_t* new = malloc(sizeof(node_t));
    assert(new);
    new->arr_time = arr_time;
    new->remain_time = remain_time;
    new->service_time = remain_time;
    new->memory = memory;
    strcpy(new->pid, pid);
    new->address_space = -1; // no memory when we initialise the process
    new->num_pages = 0;

    new->state = READY;
    new->start_time = 0;   // Initialize start_time
    new->finish_time = 0;  // Initialize finish_time

    new->next = NULL;
    return new;
}

// get the head of linked list
node_t* get_head(list_t* list){
    // it is an empty list
    if (list->head == NULL) {
        return NULL;
    }
    node_t* head = list->head;
    list->head = head->next;
    head->next = NULL;
    return head;
} 


// determine whather current list is empty or not

int is_empty_list(list_t *list) {
    if (list == NULL || list->head == NULL) {
        return 1;
    } else {

        assert(list != NULL);
        return 0;
    }
} 

// read input from file and put it into input queue
void read_input(list_t* input_queue, char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("file opend failed!\n");
    }

    int arr_time, remain_time, memory;
    char pid[MAX_LEN+1];

    while(fscanf(fp,"%d %s %d %d\n",&arr_time,pid,&remain_time,&memory) == 4){
       // one node is one process
        node_t* new = create_node(pid,arr_time,remain_time,memory);
        input_queue = insert_at_foot(input_queue,new);
    }

    // close the file
    fclose(fp);
}

// from input queue get all processes that ready to run on CPU
// store them into ready queue
void get_arr_list(list_t* input_queue, list_t* ready_queue, int time){
     // loop input queue, get process that: arr_time < time
     // add statisfied processes into ready queue and delete it from inout queue

    node_t* temp = input_queue -> head;
    while(temp != NULL){
        node_t* next = temp->next;
        if(temp->arr_time <= time){
            delete_node(input_queue, temp);
            temp->next = NULL;
            ready_queue = insert_at_foot(ready_queue, temp); 
        } 
        temp = next;
    }
}


// delete node from unknow position
void delete_node(list_t* list, node_t* node){
    // if node is the head of the list, change pointer of head directly

    if(list->head == node){
        list->head = list->head->next;
        return;
    }


    // if not the head node
    // loop whole list to find position

    node_t* prev, *curr = list->head;
    while(curr!= NULL){
        prev = curr;
        curr = curr->next;
        if (curr == node){
            break;
        }
    }
    prev->next = curr->next;

    // if the node is the foot node of list
    if(list->foot == node){
        list->foot = prev;
     }
}


void free_list(list_t* list){

    assert(list != NULL);
    node_t *curr = list->head;   // point to first node in linked list
    while (curr){                   // while current node not reaching the end of list
        node_t *tmp = curr;      
        curr = curr->next;          // point to next node

        free(tmp);
    }
    free(list);          // free list itself
} 

// use to count how many process remian in the ready queue
int count_processes(list_t *queue) {
    int count = 0;
    node_t *current = queue->head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

