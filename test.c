#include "mem_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MEMORY_SIZE 102400
#define DEFAULT_MAX_SIZE 1000
#define DEFAULT_POINTERS_SIZE 100

int main() {
    static char memory[MEMORY_SIZE];
    mem_pool *pool = mem_init(memory, sizeof(memory));
    if (pool == NULL) {
        return 1;
    }

    // Set the seed for random number generation
    srand(time(NULL));

    size_t max_size = DEFAULT_MAX_SIZE;
    size_t pointers_size = DEFAULT_POINTERS_SIZE;

    if (max_size * pointers_size > MEMORY_SIZE) {
        printf("Error: Max size and pointers size exceed memory size\n");
        return 1;
    }

    void **pointers = (void **)malloc(pointers_size * sizeof(void *));
    if (pointers == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (size_t i = 0; i < pointers_size; i++) {
        size_t size = rand() % max_size + 1;

        void *ptr = mem_malloc(pool, size);
        if (ptr == NULL) {
            printf("Memory allocation failed\n");
            return 1;
        }

        pointers[i] = ptr;
        printf("Allocated memory at %p, size = %zu\n", ptr, size);
    }

    print_used_blocks(pool);
    print_free_blocks(pool);
    print_memory_usage(pool);

    for (size_t i = 0; i < pointers_size / 2; i++) {
        int index = rand() % (int)pointers_size;
        if (pointers[index] != NULL) {
            mem_free(pool, pointers[index]);
            printf("Freed memory at %p\n", pointers[index]);
            pointers[index] = NULL;
        }
    }

    print_used_blocks(pool);
    print_free_blocks(pool);
    print_memory_usage(pool);

    for (size_t i = 0; i < pointers_size; i++) {
        if (pointers[i] != NULL) {
            mem_free(pool, pointers[i]);
            printf("Freed memory at %p\n", pointers[i]);
            pointers[i] = NULL;
        }
    }

    print_used_blocks(pool);
    print_free_blocks(pool);
    print_memory_usage(pool);

    free(pointers);

    return 0;
}
