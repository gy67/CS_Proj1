#include <stdio.h>
#include <string.h>
#include <math.h>
#include "scheduling.h"
#include "memory_management.h"


// Schedule processes and return the total time taken
int schedule_processes(list_t *input_queue, list_t *ready_queue, list_t *finished_queue, int quantum, char **strategy) {
    int time = 0;
    int memory[MEMORY_SIZE] = {0};
    int pages[TOTAL_PAGE] = {0};
    while (!is_empty_list(input_queue) || !is_empty_list(ready_queue)) {
        // Move processes from input_queue to ready_queue based on arrival time
        get_arr_list(input_queue, ready_queue, time);
        node_t* head = NULL;
        
        while (!is_empty_list(ready_queue)){ 
            // Get the head of ready_queue
            head = get_head(ready_queue);

            if (head == NULL){
                break;
            }

            if (head->address_space != ALLOCATE_FAIL){
                break;
            }

            // distinguish if the memory is allocated successfully
            int success = -1;
            
            // distinguish different strategy
            if (strcmp(*strategy, "infinite") == 0){
                break;
            }else if (strcmp(*strategy, "first-fit") == 0){
                success = first_fit(head, memory);
            }else if (strcmp(*strategy, "paged") == 0){
                success = paged_memory(ready_queue, head, pages, time);
            }else if (strcmp(*strategy, "virtual") == 0){
                success = virtual_memory(ready_queue, head, pages, time);
            }

            if (success == 1){
                break;
            }else{
                insert_at_foot(ready_queue, head);
            }  
        }

        // if there have no process in ready queue
        if (head == NULL) {
            time += quantum;
            continue;
        }

        // Set start_time if it's the first time running
        if (head->start_time == 0) {
            head->start_time = time;
        }
        
        // Process scheduling logic
        if (head == ready_queue->foot) {
            if (head->state == RUNNING) {
                head->remain_time -= quantum;
            } else if (head->state == READY) {
                head->state = RUNNING;
                print_running_info(head, time, &(*strategy), memory, pages);
                head->remain_time -= quantum;
            }
        } else {
            if (head->state == READY) {
                head->state = RUNNING;
                print_running_info(head, time, &(*strategy), memory, pages);
                head->remain_time -= quantum;
                head->state = READY;
            } else {
                print_running_info(head, time, &(*strategy), memory, pages);
                head->remain_time -= quantum;
                head->state = READY;
            }
        }

        time += quantum;

        // Move processes from input_queue to ready_queue again
        get_arr_list(input_queue, ready_queue, time);

        // If the process still has remaining time, reinsert it into the ready_queue
        if (head->remain_time > 0) {
            insert_at_foot(ready_queue, head);
        } else {
            head->finish_time = time;
            if (strcmp(*strategy, "first-fit") == 0){
                // change the state1 to state 0 which is occupied by head in memory.
                deallocate_memory_first(head, memory);
            } else if (strcmp(*strategy, "paged") == 0 || strcmp(*strategy, "virtual") == 0){
                // print and deallocate
                print_evictd_info(head, head->num_pages, time);
                deallocate_memory_paged(head, pages);
            }
            printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", time, head->pid, count_processes(ready_queue));
            head->state = FINISHED; 
            insert_at_foot(finished_queue, head);
        }
    }
    return time;
}

// print all running information
void print_running_info(node_t *process, int time, char **strategy, int *memory, int *pages) {
    node_t *head = process;
    if (strcmp(*strategy, "infinite") == 0) {
        printf("%d,RUNNING,process-name=%s,remaining-time=%d\n", time, head->pid, head->remain_time);
    } else if (strcmp(*strategy, "first-fit") == 0) {
        int memory_usage = calculate_memory_usage(memory, MEMORY_SIZE);
        printf("%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%%,allocated-at=%d\n", 
               time, head->pid, head->remain_time, memory_usage, head->address_space);
    } else if (strcmp(*strategy, "paged") == 0 || strcmp(*strategy, "virtual") == 0) {
        int mem_usage = calculate_memory_usage(pages, TOTAL_PAGE);
        printf("%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%%,mem-frames=", 
               time, head->pid, head->remain_time, mem_usage);
        print_frames(head->frames, head->num_pages);
    }
}

// calculate memory usage,if the recording number is 1 it means the memory is used
int calculate_memory_usage(int memory[], int total){
    double count_usage = 0.0;
    for (int i = 0;i < total; i++){
        if (memory[i] == FILLED){
            count_usage += 1;
        }
    }
    double usage = ceil((count_usage / total) * PERCENTAGE_NUMBER);
    return (int)usage;
}