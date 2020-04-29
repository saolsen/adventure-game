//
// Created by Stephen Olsen on 4/20/20.
//

#ifndef CODE_STEVE_H
#define CODE_STEVE_H

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))

typedef uint8_t u8;
typedef uint64_t u64;
typedef size_t usize;

typedef struct Block {
    usize used;
    usize cap;
    struct Block *prev_allocated;
    struct Block *next_free;
    u8 base[];
} Block;

typedef struct Arena {
    Block *block;
    Block *free_blocks;
    Block *last_allocated_block;
} Arena;

const usize page_size = 64 * 1024;
u8 *alloc_memory(usize num_pages);

// Add a value to the arena.
u8 *arena_push(Arena *arena, usize size);
// Reset the arena to be empty but don't free memory to os, reuse it.
void arena_reset(Arena *arena);
// Free arena memory back to the os.
void arena_free(Arena *arena);
// Print out some arena stats.
void arena_debug(Arena *arena);

extern Arena dbg_a;

typedef struct ArrayHeader {
    usize length;
    usize capacity;
    Arena *arena;
    usize padding; // @TODO: Some other debug thing can go here.
    u8 arr[];
} ArrayHeader;

#define _arrhead(a)                                                            \
    ((ArrayHeader *)((uint8_t *)a - offsetof(struct ArrayHeader, arr)))
#define _arrneedgrow(a, n) (arrlen(a) + n > arrcap(a))
#define _arrgrow(a, n) ((a) = _arrgrowf((a), arrlen(a) + (n), sizeof(*(a))))
#define _arrmaybegrow(a, n) (_arrneedgrow(a, n) ? _arrgrow(a, n) : 0)

#define arrpush(a, ...)                                                        \
    (_arrmaybegrow(a, 1), (a)[_arrhead(a)->length++] = (__VA_ARGS__))
#define arradd(a, n)                                                           \
    (_arrmaybegrow(a, n), _arrhead(a)->length += (n), &(a)[arrlen(a) - (n)])

#define arrcap(a) ((a) ? _arrhead(a)->capacity : 0)
#define arrlen(a) ((a) ? _arrhead(a)->length : 0)
#define arrend(a) (a) + arrlen(a)
#define arrclear(a) ((a) ? _arrhead(a)->length = 0 : 0)
#define arrfree(a) ((a) ? (free((ArrayHeader *)a - 1), (a) = NULL) : 0)

void *_arrgrowf_raw(void *a, size_t new_len, size_t itemsize);

#ifdef __cplusplus
template <class T> static T *_arrgrowf(T *a, uint64_t num, uint64_t itemsize) {
    return (T *)_arrgrowf_raw((void *)a, num, itemsize);
}
#else
#define _arrgrowf _arrgrowf_raw
#endif

#define arrprintf(a, ...) ((a) = arr__printf((a), __VA_ARGS__))
char *arr__printf(char *a, const char *fmt, ...);

const char *intern_range(char*** table, Arena *arena, const char *start, const char *end);
const char *intern_str(char*** table, Arena* arena, const char *str);
const char *intern_size(char*** table, Arena *arena, const char *start, usize size);

uint64_t hash(const char *str, usize len) {
    u64 h = 5381;
    for (int i = 0; i < len; i++) {
        h = ((h << 5u) + h) + str[i];
    }
    return h;
}

#endif //CODE_STEVE_H
