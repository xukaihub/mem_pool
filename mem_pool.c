#include "mem_pool.h"
#include <stdio.h>
#include <stdlib.h>

#define ALIGNMENT 4
#define BITS_PER_LINE 256

typedef struct mem_block {
    void *start;
    int size;
    int used;
    struct mem_block *next;
} mem_block;

struct mem_pool {
    mem_block *head;
};

mem_pool *mem_init(void *start, size_t size)
{
    if (start == NULL || size == 0) {
        printf("Invalid start address or size\n");
        return NULL;
    }

    mem_pool *pool = (mem_pool *)malloc(sizeof(mem_pool));
    if (pool == NULL) {
        printf("Failed to initialize memory manager\n");
        return NULL;
    }

    pool->head = (mem_block *)malloc(sizeof(mem_block));
    if (pool->head == NULL) {
        printf("Failed to initialize memory block\n");
        free(pool);
        return NULL;
    }

    pool->head->start = start;
    pool->head->size = size;
    pool->head->used = 0;
    pool->head->next = NULL;

    return pool;
}

void print_used_blocks(mem_pool *pool)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return;
    }

    printf("Used blocks:\n");
    mem_block *block = pool->head;
    while (block) {
        if (block->used) {
            printf("Start: %p, Size: %d\n", block->start, block->size);
        }
        block = block->next;
    }
}

void print_free_blocks(mem_pool *pool)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return;
    }

    printf("Free blocks:\n");
    mem_block *block = pool->head;
    while (block) {
        if (!block->used) {
            printf("Start: %p, Size: %d\n", block->start, block->size);
        }
        block = block->next;
    }
}

void *mem_malloc(mem_pool *pool, size_t size)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return NULL;
    }

    if (size == 0) {
        printf("Invalid allocation size\n");
        return NULL;
    }

    mem_block *block = pool->head;
    size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1); // round up to nearest multiple of ALIGNMENT
    while (block) {
        if (!block->used && block->size >= size) {
            // If there's enough remaining space, split it into two blocks
            if (block->size > size) {
                mem_block *new_block = (mem_block *)malloc(sizeof(mem_block));
                if (new_block == NULL) {
                    printf("Memory allocation failed\n");
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
            return block->start;
        }
        block = block->next;
    }
    return NULL;
}

void mem_free(mem_pool *pool, void *ptr)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return;
    }

    if (ptr == NULL) {
        printf("Invalid memory block\n");
        return;
    }

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

            return;
        }
        prev = block;
        block = block->next;
    }
}

void print_memory_usage(mem_pool *pool)
{
    if (pool == NULL) {
        printf("Invalid memory pool\n");
        return;
    }

    printf("Memory usage (0: free, 1: used):\n");
    char *cursor = (char *)pool->head->start;
    int bit_count = 0;
    for (mem_block *block = pool->head; block; block = block->next) {
        while (cursor < (char *)block->start) {
            printf("0");
            cursor += ALIGNMENT;
            bit_count++;
            if (bit_count == BITS_PER_LINE) {
                printf("\n");
                bit_count = 0;
            }
        }
        while (cursor < (char *)block->start + block->size) {
            printf("%d", block->used);
            cursor += ALIGNMENT;
            bit_count++;
            if (bit_count == BITS_PER_LINE) {
                printf("\n");
                bit_count = 0;
            }
        }
    }
    printf("\n");
}
