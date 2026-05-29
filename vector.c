#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"//everything is there in this header 
//lets create our own vector
vector* init_vector(size_t vk_size)
{
    vector* vec = malloc(sizeof(vector));//created a object of vector and assign it somme space 
    vec->data  = NULL;;
    vec->size = 0;
    vec->capacity = 0;
    vec->elem_size = vk_size;
    return vec;
}
void data_push(vector*vec,size_t size1 , void* data)
{
    memcpy((char*)vec->data + size1 *vec->elem_size,data,vec->elem_size);
}
void push(vector* vec, void* value, size_t elem_size)
{
    if (vec->data == NULL)
    {
        vec->capacity = 1;  // 
        vec->data = malloc(vec->elem_size * vec->capacity);
    }
    else if (vec->size >= vec->capacity)
    {
        vec->capacity = vec->capacity * 2;
        vec->data = realloc(vec->data, vec->elem_size * vec->capacity);  
    }
    data_push(vec, vec->size++, value);
}
void pop(vector* vec)
{
    if(vec->size>0)//cause what will you pop if array is already 0
    {
        vec->size--;
    }
}
void* vec_at(vector* vec, __uint32_t index)//
{
        if (index >= vec->size) {
        // Check if the provided index is out of bounds
        fprintf(stderr, "Index out of bounds\n"); // Print an error message
        exit(1); // Exit the program with an error code
    }
    return vec->data + vec->elem_size*index;
}
size_t size(vector* vec)
{
    return vec->size;
}
int is_empty(vector* vec)
{
    return vec->size == 0;
}
