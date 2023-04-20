#include "03_threads_philosophers.h"

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void init_philosopher(struct philosopher_par *philosopher, const char *name, unsigned left_fork, unsigned right_fork,
                      double time_to_eating, double time_to_thinking)
{
    philosopher->name = name;
    philosopher->left_fork = left_fork;
    philosopher->right_fork = right_fork;
    philosopher->time_to_eating = time_to_eating;
    philosopher->time_to_thinking = time_to_thinking;
}

void init_table(struct table_par *table)
{
    for (int i = 0; i < PHILOS_QUANTITY; ++i)
        pthread_mutex_init(&table->forks[i], NULL);
}

void* philosopher_eating(void *args)
{
     struct arguments_philosopher_table *arg = (struct arguments_philosopher_table *)args;
     const struct philosopher_par *philosopher = arg->philosopher;
     const struct table_par *table = arg->table;
     while(1) {
       Sleep(philosopher->time_to_thinking);
       printf("%s is hungry after %.0lf ms of thinking\n", philosopher->name, philosopher->time_to_thinking);
       clock_t begin = clock();
       pthread_mutex_lock(&mtx);    // Блокировка процесса взятия обеих вилок
       pthread_mutex_lock(&table->forks[philosopher->left_fork]);
       pthread_mutex_lock(&table->forks[philosopher->right_fork]);
       pthread_mutex_unlock(&mtx);
       clock_t end = clock();
       double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
       printf("%s start eating after %.0lf ms of starving\n", philosopher->name, time_spent * 1000);
       Sleep(philosopher->time_to_eating);
       pthread_mutex_unlock(&table->forks[philosopher->right_fork]);
       pthread_mutex_unlock(&table->forks[philosopher->left_fork]);
       printf("%s was eating for %.0lf ms\n", philosopher->name, philosopher->time_to_eating);
     }
}

int threads_philosophers_tests()
{
    pthread_t threads[PHILOS_QUANTITY];
    struct philosopher_par philosophers[PHILOS_QUANTITY];
    struct arguments_philosopher_table parameters[PHILOS_QUANTITY];
    struct table_par table;
    init_table(&table);
    srand(time(NULL));
    init_philosopher(&philosophers[0], "Doofenshmirtz", 0, 1, 1000, rand() % 1000);
    init_philosopher(&philosophers[1], "Confucius", 1, 2, 1000, rand() % 1000);
    init_philosopher(&philosophers[2], "Volk", 2, 3, 1000, rand() % 1000);
    init_philosopher(&philosophers[3], "Optimus Prime", 3, 4, 1000, rand() % 1000);
    init_philosopher(&philosophers[4], "Smurf", 4, 0, 1000, rand() % 1000);
    for (int i = 0; i < PHILOS_QUANTITY; ++i) {
        parameters[i].philosopher = &philosophers[i];
        parameters[i].table = &table;
    }
    for (int i = 0; i < PHILOS_QUANTITY; ++i)
        pthread_create(&threads[i], NULL, philosopher_eating, &parameters[i]);
    for (int i = 0; i < PHILOS_QUANTITY; ++i)
        pthread_join(threads[i], NULL);
    return 0;
}
