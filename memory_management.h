#ifndef _MEMORY_MANAGEMENT_H_
#define _MEMORY_MANAGEMENT_H_

#include "linkedList.h"

#define MEMORY_SIZE 2048
#define TOTAL_PAGE 512
#define FRAME_SIZE 4.0
#define ALLOCATE_FAIL -1
#define ALLOCATE_SUCCESS 1
#define FILLED 1
#define FREED 0
#define MIN_PAGES 4




// Functions for managing memory allocation and deallocation
int first_fit(node_t* node, int memory[]);
// For the first-fit mode, when the process is terminated, deallocate the memory which the process was occupied.
void deallocate_memory_first(node_t* node, int memory[]);
// Allocate the page memory for the processes
int paged_memory(list_t *ready_queue, node_t *process, int *pages, int time);
// Allocate the memory space for the processes in virtual memory mode
int virtual_memory(list_t *ready_queue, node_t *process, int *pages, int time);
// Function for counting available pages in the memory
int count_available_pages(int *pages);
// Function to evict frames based on LRU
void evict_frames_paged(list_t *ready_queue, int *pages, int num_pages, int time);
// For the paged memory mode,when the process is terminated, deallocate the pages of process which the process was occupied.
void deallocate_memory_paged(node_t *head, int *pages);
// print frames for the process
void print_frames(int frames[], int num_pages);
// Function to evict frames based virtual memory mode
void evict_frames_virtual(list_t *ready_queue, int *pages, int num_pages_evict, int time);
// print information if evicted any frames
void print_evictd_info(node_t *process, int num_evicts, int time);
// If evicting any frames, the other frame need to shift which equals the evicting pages number
void shiftArray(int *frames, int num_frames, int position);

#endif