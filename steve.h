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

uint64_t hash(char *str, usize len);

typedef struct {
    int x, y;
} I2;

typedef struct {
    int x, y, z;
} I3;

typedef struct {
    float x, y;
} F2;

inline F2 f2(float x, float y) { return (F2){.x = x, .y = y}; }

const F2 f2_zero = {.x = 0.0f, .y = 0.0f};
const F2 f2_one = {.x = 1.0f, .y = 1.0f};
const F2 f2_x = {.x = 1.0f, .y = 0.0f};
const F2 f2_y = {.x = 0.0f, .y = 1.0f};

typedef struct {
    float x, y, z;
} F3;

inline F3 f3(float x, float y, float z) { return (F3){.x = x, .y = y, .z = z}; }

const F3 f3_zero = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
const F3 f3_one = {.x = 1.0f, .y = 1.0f, .z = 1.0f};
const F3 f3_x = {.x = 1.0f, .y = 0.0f, .z = 0.0f};
const F3 f3_y = {.x = 0.0f, .y = 1.0f, .z = 0.0f};
const F3 f3_z = {.x = 0.0f, .y = 0.0f, .z = 1.0f};

inline F3 f2_xyz(F2 v) { return (F3){.x = v.x, .y = v.y, .z = 0.0}; }
inline F2 f3_xy(F3 v) { return (F2){.x = v.x, .y = v.y}; }

inline F2 f2_add(F2 a, F2 b) { return (F2){.x = a.x + b.x, .y = a.y + b.y}; }
inline F2 f2_sub(F2 a, F2 b) { return (F2){.x = a.x - b.x, .y = a.y - b.y}; }
inline F2 f2_mul_f(F2 v, float f) { return (F2){.x = v.x * f, .y = v.y * f}; }
inline F2 f2_div_f(F2 v, float f) { return (F2){.x = v.x / f, .y = v.y / f}; }
inline F2 f2_neg(F2 v) { return (F2){.x = -v.x, .y = -v.y}; }
inline float f2_mag2(F2 v) { return v.x * v.x + v.y * v.y; }
inline float f2_mag(F2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}
inline F2 f2_normalize(F2 v) {
    float m = f2_mag(v);
    if (m != 0.0) {
        return (F2){.x = v.x / m, .y = v.y / m};
    } else {
        return v;
    }
}

inline F3 f3_add(F3 a, F3 b) { return (F3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z}; }
inline F3 f3_sub(F3 a, F3 b) { return (F3){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z}; }
inline F3 f3_mul_f(F3 v, float f) { return (F3){.x = v.x * f, .y = v.y * f, .z = v.z * f}; }
inline F3 f3_div_f(F3 v, float f) { return (F3){.x = v.x / f, .y = v.y / f, .z = v.z / f}; }
inline F3 f3_neg(F3 v) { return (F3){.x = -v.x, .y = -v.y, .z = -v.z}; }
inline float f3_mag2(F3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
inline float f3_mag(F3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline F3 f3_normalize(F3 v) {
    float m = f3_mag(v);
    if (m != 0.0) {
        return (F3){.x = v.x / m, .y = v.y / m, .z = v.z / m};
    } else {
        return v;
    }
}

#endif //CODE_STEVE_H
