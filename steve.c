#include "steve.h"

u8 *alloc_memory(usize num_pages) {
    return (u8 *)calloc(1, num_pages * page_size);
}

uint64_t hash(const char *str, usize len) {
    u64 h = 5381;
    for (int i = 0; i < len; i++) {
        h = ((h << 5u) + h) + str[i];
    }
    return h;
}

Arena dbg_a = {0};

u8 *arena_push(Arena *arena, usize size) {
    if (!arena->block || arena->block->used + size > arena->block->cap) {
        bool found_page = false;
        Block **prev = &arena->free_blocks;
        Block *free_block = arena->free_blocks;
        while (free_block) {
            if (free_block->cap >= size) {
                *prev = free_block->next_free;
                arena->block = free_block;
                free_block->next_free = NULL;
                found_page = true;
                break;
            }
            prev = &free_block->next_free;
            free_block = free_block->next_free;
        }

        if (!found_page) {
            // Get a new block.
            usize pages_needed =
                    (size + sizeof(Block) + page_size - 1) / page_size;
            Block *new_block = (Block *)alloc_memory(pages_needed);
            new_block->used = 0;
            new_block->cap = pages_needed * page_size - sizeof(Block);
            new_block->prev_allocated = arena->last_allocated_block;
            arena->last_allocated_block = new_block;
            new_block->next_free = NULL;
            arena->block = new_block;
        }
    }

    // Push Value
    u8 *result = arena->block->base + arena->block->used;
    arena->block->used += size;
    return result;
}

// Puts all the blocks we have on a free list.
void arena_reset(Arena *arena) {
    arena->free_blocks = NULL;
    Block *block = arena->last_allocated_block;
    while (block) {
        block->used = 0;
        block->next_free = arena->free_blocks;
        arena->free_blocks = block;
        block = block->prev_allocated;
    }
    arena->block = NULL;
}

// Frees all blocks back to the os.
void arena_free(Arena *arena) {
    Block *block = arena->last_allocated_block;
    while (block) {
        Block *free_me = block;
        block = block->prev_allocated;
        free(free_me);
    }
    arena->block = NULL;
    arena->last_allocated_block = NULL;
    arena->free_blocks = NULL;
}

void arena_debug(Arena *arena) {
    if (arena->block == NULL) {
        return;
    }
    printf("base: %li\nused: %i\ncap: %i\n", (long)arena->block,
           (int)arena->block->used, (int)arena->block->cap);

    int blocks = 0;
    Block *block = arena->last_allocated_block;
    while (block) {
        blocks++;
        block = block->prev_allocated;
    }
    int free_blocks = 0;
    Block *free_block = arena->free_blocks;
    while (free_block != NULL) {
        free_blocks++;
        free_block = free_block->next_free;
    }
    printf("allocated blocks: %i, free_blocks: %i\n\n", blocks, free_blocks);
}

// Emit a graphviz file of the arena. This is super useful for debugging.
void arena_viz(Arena *arena, char *filename) {
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "digraph {\n");
    fprintf(fp, "    \"Arena\" [shape = \"record\", label=");
    fprintf(fp, "\"Arena | <f1> block | <f2> free_blocks | <f3> "
                "last_allocated_block\"");
    fprintf(fp, "];\n");

    for (Block *block = arena->last_allocated_block; block;
         block = block->prev_allocated) {
        fprintf(fp, "    \"%#010llx\" [shape = \"record\", label=", (u64)block);
        fprintf(
                fp,
                "\"<f0> Block | <f1> base: %#010llx | used: %lu | capacity: %lu | "
                "<f2> prev_allocated | <f3> next_free\"];\n",
                (u64)block, block->used, block->cap);
    }
    if (arena->block) {
        fprintf(fp, "    \"Arena\":f1 -> \"%#010llx\":f0;\n",
                (u64)arena->block);
    }
    if (arena->last_allocated_block) {
        fprintf(fp, "    \"Arena\":f3 -> \"%#010llx\":f0;\n",
                (u64)arena->last_allocated_block);
        for (Block *block = arena->last_allocated_block; block;
             block = block->prev_allocated) {
            if (block->prev_allocated != NULL) {
                fprintf(fp, "    \"%#010llx\":f2 -> \"%#010llx\":f1;\n",
                        (u64)block, (u64)block->prev_allocated);
            }
        }
    }
    if (arena->free_blocks != NULL) {
        fprintf(fp, "    \"Arena\":f2 -> \"%#010llx\":f0;\n",
                (u64)arena->free_blocks);
        for (Block *block = arena->free_blocks; block;
             block = block->next_free) {
            if (block->next_free != NULL) {
                fprintf(fp, "    \"%#010llx\":f3 -> \"%#010llx\":f1;\n",
                        (u64)block, (u64)block->next_free);
            }
        }
    }
    fprintf(fp, "}\n");
    fclose(fp);
}

void *_arrgrowf_raw(void *a, size_t new_len, size_t itemsize) {
    size_t new_cap = MAX(1 + 2 * arrcap(a), new_len);
    assert(new_len <= new_cap);
    size_t new_size = offsetof(struct ArrayHeader, arr) + new_cap * itemsize;
    ArrayHeader *new_header;
    if (a) {
        new_header = (ArrayHeader *)realloc(_arrhead(a), new_size);
    } else {
        new_header = (ArrayHeader *)malloc(new_size);
        new_header->length = 0;
    }
    new_header->capacity = new_cap;
    return new_header->arr;
}

char *arr__printf(char *a, const char *fmt, ...) {
    // Assert that our buf is null terminated if there's anything in it.
    assert(!a || arrlen(a) == 0 || arrlen(a) != 0 && a[arrlen(a) - 1] == 0);
    va_list args;
    va_start(args, fmt);
    size_t n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char *dest;
    if (arrlen(a) == 0) {
        dest = arradd(a, n + 1);
    } else {
        dest = arradd(a, n) - 1;
    }

    va_start(args, fmt);
    vsnprintf(dest, n + 1, fmt, args);
    va_end(args);
    return a;
}

// Check the string against each string in the table. If end is null we treat start as a null terminated
// string.
// If the string is already in the table return a pointer to the string in the table. Otherwise allocate
// a new string into the table and return a pointer to it.
const char *intern_range(char*** table, Arena *arena, const char *start, const char *end) {
    for (int i=0; i< arrlen(*table); i++) {
        const char *s1 = *table[i];
        const char *s2 = start;
        while (*s1 && ((end == NULL && *s2) || (end != NULL && s2 != end)) && *s1 == *s2) {
            s1++; s2++;
        }
        if (*s1 == '\0' && ((end == NULL && *s2 == '\0') || (end != NULL && s2 == end))) {
            return *table[i];
        }
    }
    usize size = end ? end - start + 1 : strlen(start) + 1;
    char *new_s = (char*)arena_push(arena, size);
    memcpy(new_s, start, size);
    new_s[size-1] = '\0';
    arrpush(*table, new_s);
    return new_s;
}

const char *intern_str(char*** table, Arena* arena, const char *str) {
    return intern_range(table, arena, str, NULL);
}

const char *intern_size(char*** table, Arena *arena, const char *start, usize size) {
    return intern_range(table, arena, start, start+size);
}