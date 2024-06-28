#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "linkedList.h"
#include "memory_management.h"
#include "scheduling.h"

#define INPUT_ARGUMENT_COUNT 7
#define GET_ARGUMENT_SPAN 2


// Function prototype
void process_args(int argc, char **argv, char **filename, char **strategy, int *quantum);
void initialize_queues(list_t **input_queue, list_t **ready_queue, list_t **finished_queue);
void calculate_statistics(list_t *finished_queue, int total_time);

int main(int argc, char **argv) {
    char *filename = NULL;
    char *strategy = NULL;
    int quantum = 0;

    // Process command line arguments
    process_args(argc, argv, &filename, &strategy, &quantum);

    // Initialize queues
    list_t *input_queue, *ready_queue, *finished_queue;
    initialize_queues(&input_queue, &ready_queue, &finished_queue);

    // Read input and schedule processes
    read_input(input_queue, filename);
    int total_time = schedule_processes(input_queue, ready_queue, finished_queue, quantum, &strategy);

    // Calculate and print statistics
    calculate_statistics(finished_queue, total_time);

    // free memory that alloctad to list
    free_list(input_queue);
    free_list(ready_queue);
    free_list(finished_queue);
    return 0;
}

// Process command line arguments
void process_args(int argc, char **argv, char **filename, char **strategy, int *quantum) {
    // Check if the correct number of arguments is provided
    if (argc != INPUT_ARGUMENT_COUNT) {
        printf("Invalid input\n");
        exit(0);
    }

    // Parse the arguments
    for (int i = 1; i < argc - 1; i += GET_ARGUMENT_SPAN) {
        if (strcmp(argv[i], "-f") == 0) {
            *filename = argv[i + 1];
        } else if (strcmp(argv[i], "-m") == 0) {
            *strategy = argv[i + 1];
        } else if (strcmp(argv[i], "-q") == 0) {
            *quantum = atoi(argv[i + 1]);
        }
    }

    // Check if all required arguments are provided
    if (*filename == NULL || *strategy == NULL || *quantum == 0) {
        // print
        fprintf(stderr, "Missing arguments\n");
        exit(0);
    }
}

// Initialize the input, ready, and finished queues
void initialize_queues(list_t **input_queue, list_t **ready_queue, list_t **finished_queue) {
    *input_queue = make_empty_list();
    *ready_queue = make_empty_list();
    *finished_queue = make_empty_list();
}



// Calculate and print statistics
void calculate_statistics(list_t *finished_queue, int total_time) {

    double max_overhead = 0.0;
    double total_overhead = 0.0;
    double total_turnaround = 0.0;
    int completed_processes = 0;
    
    node_t *current = finished_queue->head;
    while (current != NULL) {
        int turnaround_time = current->finish_time - current->arr_time;
        double time_overhead = (double)turnaround_time / current->service_time;

        if (time_overhead > max_overhead) {
            max_overhead = time_overhead;
        }

        total_overhead += time_overhead;
        total_turnaround += turnaround_time;
        completed_processes++;

        current = current->next;
    }

    double average_overhead = (completed_processes > 0) ? (total_overhead / completed_processes) : 0.0;
    double average_turnaround = (completed_processes > 0) ? (total_turnaround / completed_processes) : 0.0;
    // round up the result
    int rounded_turnaround = (int)ceil(average_turnaround);

    // print the results
    printf("Turnaround time %d\n", rounded_turnaround);

    printf("Time overhead %.2f %.2f\n", round(max_overhead * 100) / 100, round(average_overhead * 100) / 100);
    printf("Makespan %d\n", total_time);
}

