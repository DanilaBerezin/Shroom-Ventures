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
 */
void *ArenaAlloc(Arena *arena, size_t bytes);

/*
 * TODO: explanation here
 */
void DestroyArena(Arena *arena);
