#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <stddef.h>

typedef struct mem_pool mem_pool;

/**
 * Initialize the memory pool.
 *
 * @param start The starting address of the memory pool.
 * @param size The size of the memory pool.
 * @return The handle to the memory pool. Returns NULL if initialization fails.
 */
mem_pool *mem_pool_init(void *start, size_t size);

/**
 * Print the used memory blocks.
 *
 * @param pool The memory pool handle.
 */
void mem_pool_print_used_blocks(mem_pool *pool);

/**
 * Print the free memory blocks.
 *
 * @param pool The memory pool handle.
 */
void mem_pool_print_free_blocks(mem_pool *pool);

/**
 * Allocate a memory block of the specified size.
 *
 * @param pool The memory pool handle.
 * @param size The size of the memory block to allocate.
 * @return The pointer to the allocated memory block. Returns NULL if allocation fails.
 */
void *mem_pool_malloc(mem_pool *pool, size_t size);

/**
 * Free the specified memory block.
 *
 * @param pool The memory pool handle.
 * @param ptr The pointer to the memory block to free.
 */
void mem_pool_free(mem_pool *pool, void *ptr);

/**
 * Print the memory usage.
 *
 * @param pool The memory pool handle.
 */
void mem_pool_print_memory_usage(mem_pool *pool);

/**
 * Get the memory information of the memory pool.
 *
 * @param pool The memory pool handle.
 * @param total_size A pointer to a variable to store the total memory size.
 * @param free_size A pointer to a variable to store the free memory size.
 */
void mem_pool_get_memory_info(mem_pool *pool, size_t *total_size, size_t *free_size);

/**
 * Destroy the memory pool and release the associated resources.
 * Note: After calling this function, accessing the released memory pool and its associated resources should be avoided.
 *
 * @param pool The memory pool handle.
 */
void mem_pool_destroy(mem_pool *pool);

#endif /* MEM_POOL_H */
