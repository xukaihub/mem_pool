#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <stddef.h>

typedef struct mem_pool mem_pool;

/**
 * 初始化内存池。
 *
 * @param start 内存池的起始地址
 * @param size 内存池的大小
 * @return 内存池的句柄，若初始化失败则返回 NULL
 */
mem_pool *mem_init(void *start, size_t size);

/**
 * 打印已使用的内存块。
 *
 * @param pool 内存池句柄
 */
void print_used_blocks(mem_pool *pool);

/**
 * 打印空闲的内存块。
 *
 * @param pool 内存池句柄
 */
void print_free_blocks(mem_pool *pool);

/**
 * 分配指定大小的内存块。
 *
 * @param pool 内存池句柄
 * @param size 要分配的内存块大小
 * @return 分配的内存块的指针，若分配失败则返回 NULL
 */
void *mem_malloc(mem_pool *pool, size_t size);

/**
 * 释放指定的内存块。
 *
 * @param pool 内存池句柄
 * @param ptr 要释放的内存块的指针
 */
void mem_free(mem_pool *pool, void *ptr);

/**
 * 打印内存使用情况（0 表示空闲，1 表示已使用）。
 *
 * @param pool 内存池句柄
 */
void print_memory_usage(mem_pool *pool);

#endif /* MEM_POOL_H */
