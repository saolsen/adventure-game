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

struct f2 {
    float x, y;
};

inline f2 v2(float x, float y) { return {.x = x, .y = y}; }

const f2 f2_zero = {.x = 0.0f, .y = 0.0f};
const f2 f2_one = {.x = 1.0f, .y = 1.0f};
const f2 f2_x = {.x = 1.0f, .y = 0.0f};
const f2 f2_y = {.x = 0.0f, .y = 1.0f};

struct f3 {
    float x, y, z;
};

inline f3 v3(float x, float y, float z) { return {.x = x, .y = y, .z = z}; }

const f3 f3_zero = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
const f3 f3_one = {.x = 1.0f, .y = 1.0f, .z = 1.0f};
const f3 f3_x = {.x = 1.0f, .y = 0.0f, .z = 0.0f};
const f3 f3_y = {.x = 0.0f, .y = 1.0f, .z = 0.0f};
const f3 f3_z = {.x = 0.0f, .y = 0.0f, .z = 1.0f};

inline f3 v3(f2 v) { return {.x = v.x, .y = v.y, .z = 0.0}; }
inline f2 v2(f3 v) { return {.x = v.x, .y = v.y}; }

inline f2 operator+(f2 a, f2 b) { return {.x = a.x + b.x, .y = a.y + b.y}; }
inline f2 &operator+=(f2 &a, f2 b) {
    a = a + b;
    return a;
}
inline f2 operator-(f2 a, f2 b) { return {.x = a.x - b.x, .y = a.y - b.y}; }
inline f2 &operator-=(f2 &a, f2 b) {
    a = a - b;
    return a;
}
inline f2 operator*(f2 v, float f) { return {.x = v.x * f, .y = v.y * f}; }
inline f2 operator*(float f, f2 v) { return v * f; }
inline f2 &operator*=(f2 &v, float f) {
    v = v * f;
    return v;
}
inline f2 operator/(f2 v, float f) { return {.x = v.x / f, .y = v.y / f}; }
inline f2 &operator/=(f2 &v, float f) {
    v = v / f;
    return v;
}
inline f2 operator-(f2 v) { return {.x = -v.x, .y = -v.y}; }

inline float mag2(f2 v) { return v.x * v.x + v.y * v.y; }
inline float mag(f2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}
inline f2 normalize(f2 v) {
    float m = mag(v);
    if (m != 0.0) {
        return {.x = v.x / m, .y = v.y / m};
    } else {
        return v;
    }
}

inline f3 operator+(f3 a, f3 b) { return {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z}; }
inline f3 &operator+=(f3 &a, f3 b) {
    a = a + b;
    return a;
}
inline f3 operator-(f3 a, f3 b) { return {.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z}; }
inline f3 &operator-=(f3 &a, f3 b) {
    a = a - b;
    return a;
}
inline f3 operator*(f3 v, float f) { return {.x = v.x * f, .y = v.y * f, .z = v.z * f}; }
inline f3 operator*(float f, f3 v) { return v * f; }
inline f3 &operator*=(f3 &v, float f) {
    v = v * f;
    return v;
}
inline f3 operator/(f3 v, float f) { return {.x = v.x / f, .y = v.y / f, .z = v.z / f}; }
inline f3 &operator/=(f3 &v, float f) {
    v = v / f;
    return v;
}
inline f3 operator-(f3 v) { return {.x = -v.x, .y = -v.y, .z = -v.z}; }

inline float mag2(f3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
inline float mag(f3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline f3 normalize(f3 v) {
    float m = mag(v);
    if (m != 0.0) {
        return {.x = v.x / m, .y = v.y / m, .z = v.z / m};
    } else {
        return v;
    }
}

#endif //CODE_STEVE_H
