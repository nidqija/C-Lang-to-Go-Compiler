#include <stdio.h>
#include "../include/arena.h"


// create a new arena with the given capacity 
Arena arena_create(size_t capacity ){
    Arena arena;
    arena.buffer = (char *)malloc(capacity);
    arena.capacity = capacity;
    arena.offset = 0;
    return arena;
}



// function to allocate memory from the arena
// it returns a pointer to the allocated memory
void* arena_alloc(Arena* arena , size_t size){
    
    // maintain the 8 byte alignment for 64 bit architectures
    // +7 ensures that the allocation is rounded up to the next multiple of 8
    // & ~((size_t)7) clears the last 3 bits to make it a multiple of 8
    // this is required as x86 processors expect data structures to be aligned 
    size_t aligned_size = (size + 7) & ~((size_t)7);


    // if the calculated aligned size sum with current offset is more than the allocated capacity
    // return NULL
    // this prevents buffer overflow and prevent the program to crash
    if(arena->offset + aligned_size > arena->capacity){
        return NULL
    }


    // we allocate memory by calculating the offset
    // and return the pointer to the allocated memory
    void *ptr = &arena->buffer[arena->offset];

    // we update the offset by adding the aligned size
    // to make space for the next allocation
    arena->offset += aligned_size;

    // we return the pointer to the allocated memory
    return ptr;
}   


// function to deallocate the memory from the arena
void arena_free(Arena* arena){



    // check if the current arena has any memory left
    // if it does , deallocate the memory from the arena back to default state
    // this prevents memory leaks
    if(arena->buffer){
        free(arena->buffer);
        arena->buffer = NULL;
    }

    // set the capacity threshold for the memory as 0
    // set the offset 0 as well 
    arena->capacity = 0;
    arena->offset = 0;
}

