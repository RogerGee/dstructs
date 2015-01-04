/* stack.h */
#ifndef DSTRUCTS_STACK_H
#define DSTRUCTS_STACK_H
#include "dynarray.h"

struct stack
{
    struct dynamic_array s_data;
};
struct stack* stack_new();
struct stack* stack_new_ex(int initialAlloc);
void stack_free(struct stack* stk);
void stack_free_ex(struct stack* stk,destructor dstor);
static inline void stack_init(struct stack* stk)
{ dynamic_array_init(&stk->s_data); }
static inline void stack_init_ex(struct stack* stk,int initialAlloc)
{ dynamic_array_init_ex(&stk->s_data,initialAlloc); }
static inline void stack_delete(struct stack* stk)
{ dynamic_array_delete(&stk->s_data); }
static inline void stack_delete_ex(struct stack* stk,destructor dstor)
{ dynamic_array_delete_ex(&stk->s_data,dstor); }
static inline void stack_push(struct stack* stk,void* value)
{ dynamic_array_pushback(&stk->s_data,value); }
void* stack_pop(struct stack* stk);
static inline int stack_getsize(struct stack* stk)
{ return stk->s_data.da_top; }
static inline void** stack_getbuffer(struct stack* stk)
{ return stk->s_data.da_data; }
static inline int stack_is_empty(struct stack* stk)
{ return stk->s_data.da_top == 0; }

#endif
