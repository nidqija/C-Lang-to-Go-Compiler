#ifndef ARENA_H
#define ARENA_H


#include <stddef.h>

// struct to store the arena buffer required , capacity and offset for dynamic memory allocation
typedef struct Arena {
    char *buffer;
    size_t capacity;
    size_t offset;
} Arena;


// functions to manage the arena memory allocation

// function to create a new arena
Arena arena_create(size_t capacity);

// function to allocate memory from the arena
void* arena_alloc(Arena *arena , size_t size);

// function to deallocate the memory from the arena
void arena_free(Arena *arena);


#endif 