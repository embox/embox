#include <kernel/sched/sched_strategy.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel/sched.h>
#define TASK_COUNT 1500

void test_strategy_performance(void) {
    runq_t queue;
    struct schedee tasks[TASK_COUNT];
    clock_t start_time, end_time;
    runq_init(&queue);
    for (int i = 0; i < TASK_COUNT; ++i) {
        runq_item_init(&tasks[i].runq_link);
    }

    start_time = clock();
    for (int i = 0; i < TASK_COUNT; ++i) {
        runq_insert(&queue, &tasks[i]);
    }
    end_time = clock();
    printf("Insert time: %f seconds\n", (double)(end_time - start_time)/CLOCKS_PER_SEC);
    start_time = clock();
    for (int i = 0; i < TASK_COUNT; ++i) {
        runq_get_next(&queue);
    }
    end_time = clock();
    printf("get_time time: %f seconds\n", (double)(end_time - start_time)/CLOCKS_PER_SEC);
    start_time = clock();
    for (int i = 0; i < TASK_COUNT; ++i) {
        runq_extract(&queue);
    }
    end_time = clock();
    printf("Extract time: %f seconds\n", (double)(end_time - start_time)/CLOCKS_PER_SEC);
    
}