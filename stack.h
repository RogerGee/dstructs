/* stack.h */
#ifndef DSTRUCTS_STACK_H
#define DSTRUCTS_STACK_H
#include "dynarray.h"

struct stack
{
    destructor s_dstor;
    struct dynamic_array s_data;
};
struct stack* stack_new(destructor dstor);
struct stack* stack_new_ex(destructor dstor,size_t initialAlloc);
void stack_free(struct stack* stk);
void stack_init(struct stack* stk,destructor dstor);
void stack_init_ex(struct stack* stk,destructor dstor,size_t initialAlloc);
void stack_delete(struct stack* stk);
static inline void stack_push(struct stack* stk,void* value)
{ dynamic_array_pushback(&stk->s_data,value); }
int stack_pop(struct stack* stk);
void* stack_top(struct stack* stk);
static inline size_t stack_getsize(struct stack* stk)
{ return stk->s_data.da_top; }
static inline void** stack_getbuffer(struct stack* stk)
{ return stk->s_data.da_data; }
static inline size_t stack_is_empty(struct stack* stk)
{ return stk->s_data.da_top == 0; }

#endif
