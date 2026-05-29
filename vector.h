#pragma once
#include <stddef.h>
typedef struct{
    void* data;//our actual array void cause it doesnt know which data type
    size_t size;//current number of elements in array 
    size_t capacity;//our array capacity
    size_t elem_size;//size of our elemnet its needed no cause we are generating generic na  
} vector;
vector* init_vector(size_t elem_size);
void data_push(vector* vec, size_t size , void* data);
void push(vector* vec , void* value ,size_t elem_size);
void pop(vector* vec);
void* vec_at(vector* vec, __uint32_t index);
size_t size(vector* vec);
int is_empty(vector* vec);