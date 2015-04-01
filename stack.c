/* stack.c */
#include "stack.h"
#include <stdlib.h>

struct stack* stack_new(destructor dstor)
{
    struct stack* stk;
    stk = malloc(sizeof(struct stack));
    stack_init(stk,dstor);
    return stk;
}
struct stack* stack_new_ex(destructor dstor,size_t initialAlloc)
{
    struct stack* stk;
    stk = malloc(sizeof(struct stack));
    stack_init_ex(stk,dstor,initialAlloc);
    return stk;
}
void stack_free(struct stack* stk)
{
    stack_delete(stk);
    free(stk);
}
void stack_init(struct stack* stk,destructor dstor)
{
    stk->s_dstor = dstor;
    dynamic_array_init(&stk->s_data);
}
void stack_init_ex(struct stack* stk,destructor dstor,size_t initialAlloc)
{
    stk->s_dstor = dstor;
    dynamic_array_init_ex(&stk->s_data,initialAlloc);
}
void stack_delete(struct stack* stk)
{
    if (stk->s_dstor == NULL)
        dynamic_array_delete(&stk->s_data);
    else
        dynamic_array_delete_ex(&stk->s_data,stk->s_dstor);
}
int stack_pop(struct stack* stk)
{
    if (stk->s_data.da_top > 0) {
        --stk->s_data.da_top;
        if (stk->s_dstor != NULL)
            /* call destructor on popped object */
            (*stk->s_dstor)(stk->s_data.da_data[stk->s_data.da_top]);
        return stk->s_data.da_top > 0; /* true if stack is non-empty */
    }
    return 0; /* false */
}
void* stack_top(struct stack* stk)
{
    /* return value on top of stack */
    if (stk->s_data.da_top > 0)
        return stk->s_data.da_data[stk->s_data.da_top-1];
    return NULL;
}
