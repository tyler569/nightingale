
#ifndef NIGHTINGALE_QUEUE_H
#define NIGHTINGALE_QUEUE_H

#include <ng/basic.h>

struct queue_object {
    struct queue_object* next;
    char data[];
};

struct queue {
/*
    const char* object_type;
    size_t object_len;
*/

    struct queue_object* head;
    struct queue_object* tail;
};

// void queue_init(struct queue* q, const char* type, size_t len);
void queue_enqueue(struct queue* q, struct queue_object* data);
void queue_enqueue_at_front(struct queue* q, struct queue_object* data);
struct queue_object* queue_dequeue(struct queue* q);
int queue_count(struct queue* q);

#endif

