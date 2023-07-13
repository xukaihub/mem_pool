#include "mem_pool_lock.h"
#include <pthread.h>
#include <stdlib.h>

int mem_pool_lock_init(mem_pool_lock* lock)
{
    if (lock == NULL)
        return -1;

    lock->mutex = malloc(sizeof(pthread_mutex_t));
    if (lock->mutex == NULL)
        return -1;

    pthread_mutex_init(lock->mutex, NULL);

    return 0;
}

void mem_pool_lock_acquire(mem_pool_lock* lock)
{
    if (lock == NULL)
        return;

    pthread_mutex_lock(lock->mutex);
}

void mem_pool_lock_release(mem_pool_lock* lock)
{
    if (lock == NULL)
        return;

    pthread_mutex_unlock(lock->mutex);
}

void mem_pool_lock_destroy(mem_pool_lock* lock)
{
    if (lock == NULL)
        return;

    pthread_mutex_destroy(lock->mutex);

    free(lock->mutex);
}
