/* queue.h */
#ifndef DSTRUCTS_QUEUE_H
#define DSTRUCTS_QUEUE_H
#include "dynarray.h"

struct queue
{
    destructor q_dstor;
    size_t q_head;
    struct dynamic_array q_data;
};
struct queue* queue_new(destructor dstor);
struct queue* queue_new_ex(destructor dstor,size_t initialAlloc);
void queue_free(struct queue* q);
void queue_init(struct queue* q,destructor dstor);
void queue_init_ex(struct queue* q,destructor dstor,size_t initialAlloc);
void queue_delete(struct queue* q);
static inline void queue_push(struct queue* q,void* value)
{ dynamic_array_pushback(&q->q_data,value); }
int queue_pop(struct queue* q);
void queue_reorder_head(struct queue* q); /* pop off head, and push it back on */
static inline void* queue_next(struct queue* q)
{ return q->q_head < q->q_data.da_top ? q->q_data.da_data[q->q_head] : (void*)0; }
static inline size_t queue_getsize(struct queue* q)
{ return q->q_data.da_top - q->q_head; }
static inline void** queue_getbuffer(struct queue* q)
{ return q->q_data.da_data + q->q_head; }
static inline size_t queue_is_empty(struct queue* q)
{ return q->q_head == q->q_data.da_top; }

#endif
