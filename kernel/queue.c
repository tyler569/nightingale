
#include <basic.h>
#include <debug.h>
#include "queue.h"

/*
void queue_init(struct queue* q, const char* type, size_t len) {
    // q->object_type = type;
    // q->object_len = len;
    q->head = NULL;
    q->tail = NULL;
}
*/

void queue_enqueue(struct queue* q, struct queue_object* data) {
    if (q->head) {
        q->tail->next = data;
        q->tail = q->tail->next;
    } else {
        q->tail = data;
        q->head = q->tail;
    }
}

struct queue_object* queue_dequeue(struct queue* q) {
    if (!q->head) {
        return NULL;
    }

    struct queue_object* old_head = q->head;
    q->head = q->head->next;

    return old_head;
}

