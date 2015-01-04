/* queue.h */
#ifndef DSTRUCTS_QUEUE_H
#define DSTRUCTS_QUEUE_H
#include "dynarray.h"

struct queue
{
    int q_head;
    struct dynamic_array q_data;
};
struct queue* queue_new();
struct queue* queue_new_ex(int initialAlloc);
void queue_free(struct queue* q);
void queue_free_ex(struct queue* q,destructor dstor);
void queue_init(struct queue* q);
void queue_init_ex(struct queue* q,int initialAlloc);
void queue_delete(struct queue* q);
void queue_delete_ex(struct queue* q,destructor dstor);
static inline void queue_push(struct queue* q,void* value)
{ dynamic_array_pushback(&q->q_data,value); }
void* queue_pop(struct queue* q);
static inline int queue_getsize(struct queue* q)
{ return q->q_data.da_top - q->q_head; }
static inline void** queue_getbuffer(struct queue* q)
{ return q->q_data.da_data + q->q_head; }
static inline int queue_is_empty(struct queue* q)
{ return q->q_head == q->q_data.da_top; }

#endif
