#ifndef _BUFFER_H
#define _BUFFER_H

#include "dynamicarray.h"
#include "slice.h"
#include <stdio.h>
#include "allocator.h"

#ifndef char_array
CREATE_ARRAY_TYPE(char)
#endif //char_array

typedef ARRAY(char) Buffer;
void write_slice(Buffer* b,Slice str);
void write_string(Buffer* b,const char* str);
void write_string_len(Buffer* b,const char* str,int len);
void write_char(Buffer* b,char c);
void buffer_clear(Buffer* b);
Slice buffer_to_slice(Buffer* b);

#ifdef _BUFFER_IMPL
#include <string.h>
#define GROTH_FACTOR 2

#define MIN(x,y) ((x)<(y)?(x):(y))

static inline bool grow_buf(Buffer* b,size_t len){
    if(b->len+len <b->capacity){
        return true;
    }
    if(b->alloc.realloc_fn == NULL){
        return false;
    }
    int next_capacity = b->capacity;
    if(next_capacity==0){
        next_capacity=1;
    }
    while(b->len+len >= next_capacity){
        next_capacity = GROTH_FACTOR*next_capacity;
    }
    b->data = b->alloc.realloc_fn(b->alloc.userdata,b->data,next_capacity);
    if(b->data == NULL){
        return false;
    }
    b->capacity=next_capacity;
    return true;
}

 void write_slice(Buffer* b,Slice str){
    if(!grow_buf(b,str.len)){
        return;
    }
    memcpy(b->data+b->len, str.start,str.len);
    b->len+=str.len;
}

 void write_string(Buffer* b,const char* str){
    write_slice(b,slice_from(str));
}

void write_string_len(Buffer* b,const char* str,int len){
   write_slice(b,(Slice){.start=str,.len=len});
}

 void write_char(Buffer* b,char c){
    if(!grow_buf(b,1)){
        return;
    }
    b->data[b->len]=c;
    b->len++;
}


void buffer_clear(Buffer* b){
    b->len=0;
}

Slice buffer_to_slice(Buffer* b){
    return (Slice){.start=b->data,.len=b->len};
}

#endif
#endif
