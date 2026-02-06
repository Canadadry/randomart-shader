#ifndef _SLICE_H
#define _SLICE_H

#include <stdbool.h>

typedef struct{
    const char* start;
    int len;
} Slice;

bool slice_equal(Slice left, Slice right);
Slice slice_from(const char* str);

#ifdef _SLICE_IMPL
#include <string.h>

Slice slice_from(const char* str){
    return (Slice){
        .start=str,
        .len=strlen(str),
    };
}

bool slice_equal(Slice left, Slice right){
    if (left.len != right.len) return false;
    return strncmp(left.start, right.start, left.len) == 0 ;
}

#endif // _SLICE_IMPL

#endif // _SLICE_H
