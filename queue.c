/* queue.c */
#include "queue.h"
#include <stdlib.h>

struct queue* queue_new()
{
    struct queue* q;
    q = malloc(sizeof(struct queue));
    queue_init(q);
    return q;
}
struct queue* queue_new_ex(int initialAlloc)
{
    struct queue* q;
    q = malloc(sizeof(struct queue));
    queue_init_ex(q,initialAlloc);
    return q;    
}
void queue_free(struct queue* q)
{
    queue_delete(q);
    free(q);
}
void queue_free_ex(struct queue* q,destructor dstor)
{
    queue_delete_ex(q,dstor);
    free(q);
}
void queue_init(struct queue* q)
{
    q->q_head = 0;
    dynamic_array_init(&q->q_data);
}
void queue_init_ex(struct queue* q,int initialAlloc)
{
    q->q_head = 0;
    dynamic_array_init_ex(&q->q_data,initialAlloc);
}
void queue_delete(struct queue* q)
{
    dynamic_array_delete(&q->q_data);
    q->q_head = 0;
}
void queue_delete_ex(struct queue* q,destructor dstor)
{
    dynamic_array_delete_ex(&q->q_data,dstor);
    q->q_head = 0;
}
void* queue_pop(struct queue* q)
{
    if (q->q_head < q->q_data.da_top) {
        void* r = dynamic_array_getat(&q->q_data,q->q_head);
        ++q->q_head;
        if (q->q_head == q->q_data.da_top) {
            /* queue has become empty; reset head and tail to beginning 
               of the dynamic array buffer */
            q->q_head = 0;
            q->q_data.da_top = 0;
        }
        return r;
    }
    return NULL;
}
