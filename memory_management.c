#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <string.h>
#include "memory_management.h"

// if the strategy is Round-Robin Scheduling with Contiguous Memory Allocation(First-Fit), find the continous memory for current process.
int first_fit(node_t* node, int memory[]){
    int i, j;

    for (i = 0; i < MEMORY_SIZE;i++){
        int is_contiguous = 1;
        for (j = i; j < i + node->memory ;j++){
            if (memory[j] != FREED){
                is_contiguous = 0;
                break;
            }

        }
        if(is_contiguous == 1){
            node->address_space = i;
            for (j = i; j < i + node->memory ;j++){
                memory[j] = FILLED;
            }
            return 1; // is_contiguous = 1
        }
    }
    return 0;
}

// when the process is terminated, deallocate the memory which the process was occupied.
void deallocate_memory_first(node_t* node, int memory[]){
    int i;
    for (i = node->address_space ; (i < node->address_space + node->memory); i++){
        memory[i] = FREED;
    }
}

// allocate the page memory for the processes
int paged_memory(list_t *ready_queue, node_t *process, int *pages, int time){
    // number of pages requires
    int num_pages_requires = (int)ceil(process->memory/FRAME_SIZE);
    int available_pages = count_available_pages(pages);

    // current physical memory not enough to allocate
    // need to evict LRU frmaes -- always at head of ready queue 
    if (available_pages < num_pages_requires){
        evict_frames_paged(ready_queue, pages, num_pages_requires, time);
    } 
    int count = 0;
    for (int i=0; i<TOTAL_PAGE; i++){
        if (pages[i] == FREED){
            process->frames[count++] = i;
            pages[i] = FILLED;
        }
        // filled enough frames for current process
        if (count == num_pages_requires){
            process->num_pages = num_pages_requires;
            process->address_space = ALLOCATE_SUCCESS;
            break;
        }
    }
    return 1;
}

// count the available page number currently
int count_available_pages(int *pages){
    int count = 0;
    for (int i=0; i<TOTAL_PAGE; i++){
        if (pages[i] == FREED){
            count++;
        }
    }
    return count;
}

// Function to evict frames based on LRU
void evict_frames_paged(list_t *ready_queue, int *pages, int num_pages_requires, int time){
    // while current available number of pages not enough for current process, keep evicting
    int flag = 0; // indicate if more than one process need to evict, only print first evict event
    node_t *head = ready_queue->head;
    while (head != NULL){
        if (head->address_space != ALLOCATE_FAIL){
            for (int i=0; i<head->num_pages; i++){
                pages[head->frames[i]] = FREED;
            }
            if (flag == 0){
                print_evictd_info(head, head->num_pages, time);
                flag = 1;   // turn it to 1: indicate the evict info has been printed
            }
            head->num_pages = 0;
            head->address_space = ALLOCATE_FAIL;
        }
        head = head->next;
    }
}

// For the paged memory mode,when the process is terminated, deallocate the pages of process which the process was occupied.
void deallocate_memory_paged(node_t *head, int *pages){
    for (int i=0; i<head->num_pages; i++){
        pages[head->frames[i]] = FREED;
    }
}

// print frames for the process
void print_frames(int frames[], int num_pages) {
    printf("[");
    for (int i = 0; i < num_pages; i++) {
        printf("%d", frames[i]);
        if (i < num_pages - 1) {
            printf(",");
        }
    }
    printf("]\n");
}

// print information if evicted any frames
void print_evictd_info(node_t *process, int num_evicts, int time){
    printf("%d,EVICTED,evicted-frames=", time);
    print_frames(process->frames, num_evicts);
}


// allocate the memory space for the processes in virtual memory mode
int virtual_memory(list_t *ready_queue, node_t *process, int *pages, int time){
    // number of pages requires
    int num_pages_requires = (int)ceil(process->memory/FRAME_SIZE);
    int available_pages = count_available_pages(pages);
    int num_pages_evict;
    if (num_pages_requires < MIN_PAGES){
        num_pages_evict = num_pages_requires - count_available_pages(pages);
    } else {
        num_pages_evict = MIN_PAGES - count_available_pages(pages);
    }

    // current physical memory not enough to allocate, or available pages smaller than 4
    // eg: require pages: 3, current available: 2
    // eg: require pages: 3, current available: 3 -- can start allocation
    // eg: require pages: 7, current available: 3
    // eg: require pages: 7, current available: 4 -- can start allocation 
    if ((num_pages_requires < MIN_PAGES && available_pages < num_pages_requires) || 
        (num_pages_requires >= MIN_PAGES && available_pages < MIN_PAGES)){
        evict_frames_virtual(ready_queue, pages, num_pages_evict, time);
    }
    int count = 0;
    for (int i=0; i<TOTAL_PAGE; i++){
        if (pages[i] == FREED){
            process->frames[count++] = i;
            pages[i] = FILLED;
        }
        // check available pages; if > 0 keep allocate, else stop
        if (count == num_pages_requires){
            process->num_pages = count;
            process->address_space = ALLOCATE_SUCCESS;
            break;
        } 

        if (count_available_pages(pages) <= 0){
            process->num_pages = count;
            process->address_space = ALLOCATE_SUCCESS;
            break;
        }
    }
    return 1;
}

// Function to evict frames based virtual memory mode
void evict_frames_virtual(list_t *ready_queue, int *pages, int num_pages_evict, int time){
    // while current available number of pages not enough for current process, keep evicting
    int flag = 0; // indicate if more than one process need to evict, only print first evict event
    // count number of pages that need to evict
    // first situation: requires < 4 pages, available < reuqires
   
    // start evict
    node_t *head = ready_queue->head;
    while (head != NULL && num_pages_evict > 0){
        int num_evicted = 0;

        // current process haven't been allocate sucessfully
        if (head->address_space != ALLOCATE_FAIL){
            for (int i=0; i<head->num_pages; i++){
                pages[head->frames[i]] = FREED;
                num_pages_evict--;
                num_evicted++;
                // no page was evicted
                if (num_pages_evict == 0){
                    break;
                }
            }

            if (flag == 0){
                print_evictd_info(head, num_evicted, time);
                flag = 1;   // turn it to 1: indicate the evict info has been printed
            }
                
            // left shift head->frames with num_evicted position
            shiftArray(head->frames, head->num_pages, num_evicted);

            // check current process have enough pages to run
            if (head->num_pages <= MIN_PAGES && num_evicted > 0){
                head->address_space = ALLOCATE_FAIL;
            } else if (head->num_pages > MIN_PAGES && num_evicted > 0){
                if (head->num_pages < MIN_PAGES){
                    head->address_space = ALLOCATE_FAIL;
                }
            }
            head->num_pages -= num_evicted;
        }
        head = head->next;
    }
}

// If evicting any frames, the other frame need to shift which equals the evicting pages number
void shiftArray(int *frames, int num_frames, int position){
    int length = num_frames-position;
    int temp[length];
    for (int i=0; i<length; i++){
        temp[i] = frames[position+i];
    }
    memcpy(frames, temp, length * sizeof(int));
}