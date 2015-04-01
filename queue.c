/* queue.c */
#include "queue.h"
#include <stdlib.h>

struct queue* queue_new(destructor dstor)
{
    struct queue* q;
    q = malloc(sizeof(struct queue));
    queue_init(q,dstor);
    return q;
}
struct queue* queue_new_ex(destructor dstor,size_t initialAlloc)
{
    struct queue* q;
    q = malloc(sizeof(struct queue));
    queue_init_ex(q,dstor,initialAlloc);
    return q;    
}
void queue_free(struct queue* q)
{
    queue_delete(q);
    free(q);
}
void queue_init(struct queue* q,destructor dstor)
{
    q->q_dstor = dstor;
    q->q_head = 0;
    dynamic_array_init(&q->q_data);
}
void queue_init_ex(struct queue* q,destructor dstor,size_t initialAlloc)
{
    q->q_dstor = dstor;
    q->q_head = 0;
    dynamic_array_init_ex(&q->q_data,initialAlloc);
}
void queue_delete(struct queue* q)
{
    if (q->q_dstor != NULL) {
        size_t i;
        for (i = q->q_head;i < q->q_data.da_top;++i)
            (*q->q_dstor)(q->q_data.da_data[i]);
    }
    dynamic_array_delete(&q->q_data);
    q->q_head = 0;
}
int queue_pop(struct queue* q)
{
    if (q->q_head < q->q_data.da_top) {
        if (q->q_dstor != NULL) /* call destructor */
            (*q->q_dstor)(q->q_data.da_data[q->q_head]);
        ++q->q_head;
        if (q->q_head == q->q_data.da_top) {
            /* queue has become empty; reset head and tail to beginning 
               of the dynamic array buffer */
            q->q_head = 0;
            q->q_data.da_top = 0;
            return 0; /* false */
        }
        return 1; /* true, queue is still non-empty */
    }
    return 0; /* false */
}
