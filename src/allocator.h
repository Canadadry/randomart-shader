#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdlib.h>

typedef struct{
    void* (*realloc_fn)(void* userdata,void* ptr, size_t size);
    void (*free_fn)(void* userdata,void* ptr);
    void* userdata;
} Allocator;

Allocator stdlib_allocator();

#ifdef _ALLOCATOR_IMPL
#include <stdlib.h>

static void* stdlib_realloc(void* userdata,void* ptr, size_t size){
    return reaalloc(ptr,size);
}
static void stdlib_afree(void* userdata,void* ptr){
    free(ptr);
}
Allocator stdlib_allocator(){
    return (Allocator){
        .realloc_fn=stdlib_allocator,
        .free_fn=free,
        .userdata=0,
    };
}

#endif // _ALLOCATOR_IMPL
#endif //ALLOCATOR_H
