#pragma once
#ifndef _LIST_H_
#define _LIST_H_

struct list {
        struct list *next;
        struct list *previous;
};

typedef struct list list;
typedef struct list list_head;
typedef struct list list_n;
typedef struct list list_node;

#define LIST_INIT(name) { &(name), &(name) }
#define LIST_DEFINE(name) list name = LIST_INIT(name)

#define container_of(type, node, ptr) \
        (type *)((char *)(ptr) - offsetof(type, node))

#define list_head(type, node, ptr) \
        container_of(type, node, (ptr)->next)
#define list_next list_head

#define list_for_each(type, var, list, node) \
        for ( \
                type *var   = list_head(type, node, (list)), \
                *__tmp = list_next(type, node, &var->node); \
                &var->node != list; \
                var = __tmp, \
                __tmp = list_next(type, node, &__tmp->node) \
        )

static inline
void list_insert(struct list *before, struct list *after, struct list *new_node) {
        before->next = new_node;
        new_node->previous = before;

        new_node->next = after;
        after->previous = new_node;
}

static inline
void list_append(struct list *head, struct list *new_node) {
        list_insert(head->previous, head, new_node);
}

static inline
void list_prepend(struct list *head, struct list *new_node) {
        list_insert(head, head->next, new_node);
}

static inline
void list_init(struct list *head) {
        head->next = head;
        head->previous = head;
}

static inline
void list_remove_between(struct list *previous, struct list *next) {
        next->previous = previous;
        previous->next = next;
}

static inline
void list_remove(struct list *node) {
        list_remove_between(node->previous, node->next);

        node->next = 0;
        node->previous = 0;
}

#endif
