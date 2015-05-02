/* dynarray.c */
#include "dynarray.h"
#include <stdlib.h>
#include <string.h>

struct dynamic_array* dynamic_array_new()
{
    struct dynamic_array* arr;
    arr = malloc(sizeof(struct dynamic_array));
    dynamic_array_init(arr);
    return arr;
}
struct dynamic_array* dynamic_array_new_ex(size_t initialAlloc)
{
    struct dynamic_array* arr;
    arr = malloc(sizeof(struct dynamic_array));
    dynamic_array_init_ex(arr,initialAlloc);
    return arr;
}
void dynamic_array_free(struct dynamic_array* arr)
{
    if (arr != NULL) {
        dynamic_array_delete(arr);
        free(arr);
    }
}
void dynamic_array_free_ex(struct dynamic_array* arr,destructor dstor)
{
    if (arr != NULL) {
        size_t i;
        for (i = 0;i < arr->da_top;++i)
            (*dstor)(arr->da_data[i]);
        dynamic_array_delete(arr);
        free(arr);
    }
}
void dynamic_array_init(struct dynamic_array* arr)
{
    arr->da_top = 0;
    arr->da_cap = 16;
    arr->da_data = malloc(sizeof(void*) * arr->da_cap);
}
void dynamic_array_init_ex(struct dynamic_array* arr,size_t initialAlloc)
{
    arr->da_top = 0;
    arr->da_cap = initialAlloc;
    arr->da_data = malloc(sizeof(void*) * arr->da_cap);
}
void dynamic_array_delete(struct dynamic_array* arr)
{
    free(arr->da_data);
    arr->da_data = NULL;
    arr->da_cap = 0;
    arr->da_top = 0;
}
void dynamic_array_delete_ex(struct dynamic_array* arr,destructor dstor)
{
    size_t i;
    for (i = 0;i < arr->da_top;++i)
        (*dstor)(arr->da_data[i]);
    free(arr->da_data);
    arr->da_data = NULL;
    arr->da_cap = 0;
    arr->da_top = 0;
}
static void dynamic_array_grow(struct dynamic_array* arr)
{
    size_t newsz;
    void** newarr;
    newsz = arr->da_cap==0 ? 16 : arr->da_cap<<1;
    newarr = realloc(arr->da_data,sizeof(void*) * newsz);
    arr->da_cap = newsz;
    arr->da_data = newarr;
}
void dynamic_array_pushback(struct dynamic_array* arr,void* element)
{
    if (arr->da_top >= arr->da_cap)
        dynamic_array_grow(arr);
    arr->da_data[arr->da_top++] = element;
}
void dynamic_array_pushback_ex(struct dynamic_array* arr,void** elements,size_t size)
{
    size_t i;
    for (i = 0;i < size;++i)
        dynamic_array_pushback(arr,elements[i]);
}
void dynamic_array_clear_ex(struct dynamic_array* arr,destructor dstor)
{
    size_t i;
    for (i = 0;i < arr->da_top;++i)
        (*dstor)(arr->da_data[i]);
    arr->da_top = 0;
}

void grow_dynamic_array(void** array,size_t elemcnt,size_t elemsize,size_t* capacity)
{
    void* arrnew;
    size_t capnew;
    capnew = *capacity * 2;
    arrnew = malloc(elemsize * capnew);
    memcpy(arrnew,array,elemsize*elemcnt);
    *capacity = capnew;
    free(*array);
    *array = arrnew;
}
