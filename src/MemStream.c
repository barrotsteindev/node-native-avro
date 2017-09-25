//
//  MemStream.c
//  binding
//
//  Created by Bar Rotstein on 9/14/17.
//
//

#include <stdio.h>
#include <stdlib.h>
#include "MemStream.h"

struct mem {
    size_t pos;
    size_t * size;
    char * * buffer;
};
typedef struct mem mem_t;

static int func_write(void * handler, const char * buf, int size) {
    mem_t * mem = handler;
    size_t available = * mem->size - mem->pos;
    size_t buf_size = size * sizeof(char);
    
    if (size > available) {
        size_t new_size = * mem->size + buf_size;
        char * tmp_buffer = realloc(* mem->buffer, new_size + 1);
        if (tmp_buffer == NULL) {
            return -1;
        }
        * mem->size += buf_size;
        * mem->buffer = tmp_buffer;
    }
    if (mem->buffer == NULL) {
        return -1;
    } else {
        memcpy(* mem->buffer, buf, size);
    }
    mem->pos += size; 
    return size;
}

static int closefn(void * handler) {
    free(handler);
    return 0;
}

extern FILE * CreateMemStream(char * * buf, size_t * size) {
    FILE * fileStream = NULL;
    mem_t * mem = malloc(sizeof(mem_t));
    mem->size = size;
    mem->pos = 0;
    mem->buffer = buf;
    * mem->buffer = malloc(sizeof(char));
    * mem->size = sizeof(char);
    
    fileStream = funopen(mem, NULL, func_write, NULL, closefn);
    if (fileStream == NULL) {
        free(mem);
    }
    return fileStream;
}
