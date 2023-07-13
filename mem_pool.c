#include "mem_pool.h"
#include "mem_pool_lock.h"
#include <stdio.h>
#include <stdlib.h>

#define ALIGNMENT 4
#define BITS_PER_LINE 256

typedef struct mem_block {
    void *start;
    size_t size;
    int used;
    struct mem_block *next;
} mem_block;

struct mem_pool {
    void *start;
    size_t size;
    mem_block *head;
    mem_pool_lock *lock;
};

mem_pool *mem_pool_init(void *start, size_t size)
{
    if (start == NULL || size == 0) {
        printf("Invalid start address or size\n");
        return NULL;
    }

    mem_pool *pool = (mem_pool *)malloc(sizeof(mem_pool));
    if (pool == NULL) {
        printf("Failed to initialize memory pool\n");
        return NULL;
    }
    pool->start = start;
    pool->size = size;
    pool->head = NULL;

    mem_block *block = (mem_block *)malloc(sizeof(mem_block));
    if (block == NULL) {
        printf("Failed to initialize memory block\n");
        free(pool);
        return NULL;
    }
    block->start = start;
    block->size = size;
    block->used = 0;
    block->next = NULL;

    pool->head = block;

    pool->lock = (mem_pool_lock*)malloc(sizeof(mem_pool_lock));
    if (pool->lock == NULL) {
        printf("Failed to initialize memory pool lock");
        free(block);
        free(pool);
        return NULL;
    }
    mem_pool_lock_init(pool->lock);

    return pool;
}

void mem_pool_print_used_blocks(mem_pool *pool)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return;
    }

    printf("Used blocks:\n");
    mem_block *block = pool->head;
    mem_pool_lock_acquire(pool->lock);
    while (block) {
        if (block->used) {
            printf("Start: %p, Size: %zu\n", block->start, block->size);
        }
        block = block->next;
    }
    mem_pool_lock_release(pool->lock);
}

void mem_pool_print_free_blocks(mem_pool *pool)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return;
    }

    printf("Free blocks:\n");
    mem_block *block = pool->head;
    mem_pool_lock_acquire(pool->lock);
    while (block) {
        if (!block->used) {
            printf("Start: %p, Size: %zu\n", block->start, block->size);
        }
        block = block->next;
    }
    mem_pool_lock_release(pool->lock);
}

void *mem_pool_malloc(mem_pool *pool, size_t size)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return NULL;
    }

    if (size == 0) {
        printf("Invalid allocation size\n");
        return NULL;
    }

    mem_pool_lock_acquire(pool->lock);

    mem_block *block = pool->head;
    size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1); // round up to nearest multiple of ALIGNMENT
    while (block) {
        if (!block->used && block->size >= size) {
            // If there's enough remaining space, split it into two blocks
            if (block->size > size) {
                mem_block *new_block = (mem_block *)malloc(sizeof(mem_block));
                if (new_block == NULL) {
                    printf("Memory allocation failed\n");
                    mem_pool_lock_release(pool->lock);
                    return NULL;
                }

                new_block->start = (char *)block->start + size;
                new_block->size = block->size - size;
                new_block->used = 0;
                new_block->next = block->next;

                block->size = size;
                block->next = new_block;
            }

            block->used = 1;
            mem_pool_lock_release(pool->lock);
            return block->start;
        }
        block = block->next;
    }

    mem_pool_lock_release(pool->lock);
    return NULL;
}

void mem_pool_free(mem_pool *pool, void *ptr)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return;
    }

    if (ptr == NULL) {
        printf("Invalid memory block\n");
        return;
    }

    mem_pool_lock_acquire(pool->lock);

    mem_block *block = pool->head;
    mem_block *prev = NULL;
    while (block) {
        if (block->start == ptr) {
            block->used = 0;

            // Merge adjacent free blocks
            if (block->next && !block->next->used) {
                mem_block *next = block->next;
                block->size += next->size;
                block->next = next->next;
                free(next);
            }
            if (prev && !prev->used) {
                prev->size += block->size;
                prev->next = block->next;
                free(block);
            }

            mem_pool_lock_release(pool->lock);
            return;
        }
        prev = block;
        block = block->next;
    }
}

#define USED_SYMBOL       '#'
#define UNUSED_SYMBOL     '.'
#define PRINT_BLOCK(flag) printf("%c", (flag) ? USED_SYMBOL : UNUSED_SYMBOL)

void mem_pool_print_memory_usage(mem_pool *pool)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return;
    }

    printf("Memory usage (%c: free, %c: used):\n", UNUSED_SYMBOL, USED_SYMBOL);
    char *cursor = (char *)pool->head->start;
    int bit_count = 0;
    mem_pool_lock_acquire(pool->lock);
    for (mem_block *block = pool->head; block; block = block->next) {
        while (cursor < (char *)block->start) {
            PRINT_BLOCK(0);
            cursor += ALIGNMENT;
            bit_count++;
            if (bit_count == BITS_PER_LINE) {
                printf("\n");
                bit_count = 0;
            }
        }
        while (cursor < (char *)block->start + block->size) {
            PRINT_BLOCK(block->used);
            cursor += ALIGNMENT;
            bit_count++;
            if (bit_count == BITS_PER_LINE) {
                printf("\n");
                bit_count = 0;
            }
        }
    }
    mem_pool_lock_release(pool->lock);
    printf("\n");
}

void mem_pool_get_memory_info(mem_pool *pool, size_t *total_size, size_t *free_size)
{
    if (pool == NULL || total_size == NULL || free_size == NULL) {
        printf("Invalid arguments\n");
        return;
    }

    mem_pool_lock_acquire(pool->lock);
    *total_size = pool->size;

    size_t used_size = 0;
    mem_block *block = pool->head;
    while (block) {
        if (block->used) {
            used_size += block->size;
        }
        block = block->next;
    }
    mem_pool_lock_release(pool->lock);

    *free_size = pool->size - used_size;
}

void mem_pool_destroy(mem_pool *pool)
{
    if (pool == NULL) {
        return;
    }

    mem_pool_lock_acquire(pool->lock);

    mem_block *block = pool->head;
    while (block) {
        mem_block *next = block->next;
        free(block);
        block = next;
    }

    mem_pool_lock_release(pool->lock);

    mem_pool_lock_destroy(pool->lock);
    pool->lock = NULL;

    free(pool);
}
