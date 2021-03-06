/* dynarray.h */
#ifndef DYNARRAY_H
#define DYNARRAY_H
#include <stdlib.h>
#include "dstructs.h"

/* represents a dynamically sizing array of objects; each object is stored
   by reference; a destructor can be provided to delete an object when the
   array is being freed or cleared */
struct dynamic_array
{
    size_t da_top;
    size_t da_cap;
    void** da_data;
};
struct dynamic_array* dynamic_array_new();
struct dynamic_array* dynamic_array_new_ex(size_t initialAlloc);
void dynamic_array_free(struct dynamic_array* arr);
void dynamic_array_free_ex(struct dynamic_array* arr,destructor dstor);
void dynamic_array_init(struct dynamic_array* arr);
void dynamic_array_init_ex(struct dynamic_array* arr,size_t initialAlloc);
void dynamic_array_delete(struct dynamic_array* arr);
void dynamic_array_delete_ex(struct dynamic_array* arr,destructor dstor);
void dynamic_array_pushback(struct dynamic_array* arr,void* element);
void dynamic_array_pushback_ex(struct dynamic_array* arr,void** elements,size_t size);
static inline void* dynamic_array_getat(struct dynamic_array* arr,size_t index)
{ return arr->da_data[index]; }
static inline size_t dynamic_array_getsize(struct dynamic_array* arr)
{ return arr->da_top; }
static inline void dynamic_array_clear(struct dynamic_array* arr)
{ arr->da_top = 0; }
void dynamic_array_clear_ex(struct dynamic_array* arr,destructor dstor);

/* this function will resize any array with elements of an arbitrary size */
void grow_dynamic_array(void** array,size_t elemcnt,size_t elemsize,size_t* capacity);

#endif
