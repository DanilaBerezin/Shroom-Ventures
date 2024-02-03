#include "stdbool.h"
#include "stddef.h"

typedef struct arena {
    void    *buff;
    size_t  count;
    size_t  capacity;
    bool    err;
} Arena;

/*
 * TODO: explanation here
 */
void CreateArena(Arena *arena, size_t bytes);

/*
 * TODO: explanation here
 * No check for null ptrs here, your problem if you pass one
 */
void *ArenaAlloc(Arena *arena, size_t bytes);

/*
 * TODO: explanation here
 * No check for null ptrs here, your problem if you pass one
 */
void DestroyArena(Arena *arena);
