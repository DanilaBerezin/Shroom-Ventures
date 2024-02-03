#include "arena.h"
#include "stdlib.h"

// Note: not really a full arena allocator implementation, but it will do for now.

void CreateArena(Arena *arena, size_t bytes) {
    void *buff = malloc(bytes);
    if (buff == NULL) {
        arena->err = true;
        return;
    }
    
    arena->buff = buff;
    arena->count = 0;
    arena->capacity = bytes; 
}

void *ArenaAlloc(Arena *arena, size_t bytes) {
    if (arena->count + bytes > arena->capacity) {
        arena->err = true;
        return NULL;
    }
    
    void *ptr = &((char *) arena->buff)[arena->count]; 
    arena->count += bytes;
    return ptr;
}

// No check for null ptr, it's your problem if you pass a null ptr here
void DestroyArena(Arena *arena) {
    free(arena->buff);
}
