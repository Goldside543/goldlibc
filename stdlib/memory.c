// SPDX-License-Identifier: LGPL-2.1-only
/*
 * stdlib/memory.c
 * 
 * Memory related functions.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <string.h>
#include <stddef.h>
#include <stdint.h>

#define MEMORY_POOL_SIZE (1024 * 1024 * 128)
#define ALIGNMENT 8
#define MAGIC_HEAD 0xDEADBEEF
#define MAGIC_TAIL 0xBAADF00D
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct block_header {
    uint32_t magic_head;
    size_t size;
    int free;
    struct block_header* next;
} block_header;

typedef struct block_footer {
    size_t size;
    uint32_t magic_tail;
} block_footer;

static unsigned char memory_pool[MEMORY_POOL_SIZE];
static block_header* free_list = NULL;

void write_footer(block_header* block) {
    block_footer* footer = (block_footer*)((uint8_t*)block + sizeof(block_header) + block->size);
    footer->size = block->size;
    footer->magic_tail = MAGIC_TAIL;
}

block_footer* get_footer(block_header* block) {
    return (block_footer*)((uint8_t*)block + sizeof(block_header) + block->size);
}

block_header* get_next_block(block_header* block) {
    return (block_header*)((uint8_t*)block + sizeof(block_header) + block->size + sizeof(block_footer));
}

block_header* get_prev_block(block_header* block) {
    block_footer* prev_footer = (block_footer*)((uint8_t*)block - sizeof(block_footer));
    if (prev_footer->magic_tail != MAGIC_TAIL) return NULL;
    block_header* prev_block = (block_header*)((uint8_t*)block - prev_footer->size - sizeof(block_header) - sizeof(block_footer));
    if (prev_block->magic_head != MAGIC_HEAD) return NULL;
    return prev_block;
}

void init_heap() {
    free_list = (block_header*)memory_pool;
    free_list->magic_head = MAGIC_HEAD;
    free_list->size = MEMORY_POOL_SIZE - sizeof(block_header) - sizeof(block_footer);
    free_list->free = 1;
    free_list->next = NULL;
    write_footer(free_list);
}

void* malloc(size_t size) {
    size = ALIGN(size);
    block_header* current = free_list;

    while (current) {
        if (current->free && current->size >= size) {
            size_t remaining = current->size - size;

            if (remaining >= sizeof(block_header) + sizeof(block_footer) + ALIGNMENT) {
                // Split the block
                block_header* new_block = (block_header*)((uint8_t*)current + sizeof(block_header) + size + sizeof(block_footer));
                new_block->magic_head = MAGIC_HEAD;
                new_block->size = remaining - sizeof(block_header) - sizeof(block_footer);
                new_block->free = 1;
                new_block->next = current->next;
                write_footer(new_block);

                current->size = size;
                current->next = new_block;
                write_footer(current);
            }

            current->free = 0;
            return (void*)((uint8_t*)current + sizeof(block_header));
        }

        current = current->next;
    }

    return NULL; // Out of memory
}

void free(void* ptr) {
    if (!ptr) return;

    block_header* block = (block_header*)((uint8_t*)ptr - sizeof(block_header));

    // Check for corruption
    if (block->magic_head != MAGIC_HEAD || get_footer(block)->magic_tail != MAGIC_TAIL) {
        // Corrupted block
        return;
    }

    block->free = 1;

    // Coalesce with next block if possible
    block_header* next = get_next_block(block);
    if ((uint8_t*)next < memory_pool + MEMORY_POOL_SIZE &&
        next->magic_head == MAGIC_HEAD && next->free &&
        get_footer(next)->magic_tail == MAGIC_TAIL) {

        block->size += sizeof(block_header) + next->size + sizeof(block_footer);
        block->next = next->next;
        write_footer(block);
    }

    // Coalesce with previous block if possible
    block_header* prev = get_prev_block(block);
    if (prev && prev->free) {
        prev->size += sizeof(block_header) + block->size + sizeof(block_footer);
        prev->next = block->next;
        write_footer(prev);
    } else {
        write_footer(block);
    }
}

void *realloc(void *ptr, size_t new_size) {
    if (!ptr) {
        // If ptr is NULL, just allocate new memory
        return malloc(new_size);
    }
    
    if (new_size == 0) {
        // If new_size is 0, free the memory and return NULL
        free(ptr);
        return NULL;
    }

    // Allocate new memory block
    void *new_ptr = malloc(new_size);
    if (!new_ptr) {
        return NULL;  // Failed to allocate
    }

    // Copy old data to new block (only up to the new size)
    memcpy(new_ptr, ptr, new_size);  

    // Free old block
    free(ptr);

    return new_ptr;
}
