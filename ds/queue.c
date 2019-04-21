
#include <ng/basic.h>
#include <ng/print.h>
#include <ds/queue.h>

/*
void queue_init(struct queue* q, const char* type, size_t len) {
    // q->object_type = type;
    // q->object_len = len;
    q->head = NULL;
    q->tail = NULL;
}
*/

void print_queue(struct queue* q) {
    struct queue_object* qo;

    for (qo=q->head; qo; qo=qo->next) {
        printf("qo = %p\n", qo);
    }
    printf("q->tail = %p\n", q->tail);
}


void queue_enqueue(struct queue* q, struct queue_object* data) {
    data->next = NULL;

    if (q->head) {
        q->tail->next = data;
        q->tail = q->tail->next;
    } else {
        q->tail = data;
        q->head = q->tail;
    }
}

void queue_enqueue_at_front(struct queue* q, struct queue_object* data) {
    data->next = NULL;

    if (q->head) {
        struct queue_object* old_head = q->head;
        data->next = old_head;
        q->head = data;
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

int queue_count(struct queue* q) {
    struct queue_object* qo = NULL;
    int count = 0;

    if (!(qo = q->head)) {
        return 0;
    }

    while ((qo = qo->next)) {
        count++;
    }

    return count;
}

