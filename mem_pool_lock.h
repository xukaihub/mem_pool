#ifndef MEM_POOL_LOCK_H
#define MEM_POOL_LOCK_H

typedef struct mem_pool_lock {
    void *mutex;
} mem_pool_lock;

typedef struct mem_pool_lock mem_pool_lock;

int mem_pool_lock_init(mem_pool_lock* lock);
void mem_pool_lock_acquire(mem_pool_lock* lock);
void mem_pool_lock_release(mem_pool_lock* lock);
void mem_pool_lock_destroy(mem_pool_lock* lock);

#endif
