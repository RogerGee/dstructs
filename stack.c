/* stack.c */
#include "stack.h"
#include <stdlib.h>

struct stack* stack_new()
{
    struct stack* stk;
    stk = malloc(sizeof(struct stack));
    stack_init(stk);
    return stk;
}
struct stack* stack_new_ex(size_t initialAlloc)
{
    struct stack* stk;
    stk = malloc(sizeof(struct stack));
    stack_init_ex(stk,initialAlloc);
    return stk;
}
void stack_free(struct stack* stk)
{
    stack_delete(stk);
    free(stk);
}
void stack_free_ex(struct stack* stk,destructor dstor)
{
    stack_delete_ex(stk,dstor);
    free(stk);
}
void* stack_pop(struct stack* stk)
{
    if (stk->s_data.da_top > 0) {
        --stk->s_data.da_top;
        return dynamic_array_getat(&stk->s_data,stk->s_data.da_top);
    }
    return NULL;
}
void* stack_top(struct stack* stk)
{
    if (stk->s_data.da_top > 0)
        return dynamic_array_getat(&stk->s_data,stk->s_data.da_top);
    return NULL;
}
