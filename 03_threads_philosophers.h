#ifndef THREADS_PHILOSOPHER
#define THREADS_PHILOSOPHER

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define PHILOS_QUANTITY 5

struct philosopher_par {
    const char *name;
    unsigned left_fork;
    unsigned right_fork;
    double time_to_eating;
    double time_to_thinking;
};

struct table_par {
    pthread_mutex_t forks[PHILOS_QUANTITY];
};

struct arguments_philosopher_table {
    const struct philosopher_par *philosopher;
    const struct table_par *table;
};

void init_philosopher(struct philosopher_par *philosopher, const char *name, unsigned left_fork, unsigned right_fork,
                      double time_to_eating, double time_to_thinking);
void init_table(struct table_par *table);
void* philosopher_eating(void *args);
int threads_philosophers_tests();

#endif
