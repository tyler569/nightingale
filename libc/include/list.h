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

#define LIST_INIT(name)                                                        \
    { &(name), &(name) }
#define LIST_DEFINE(name) list name = LIST_INIT(name)

#define container_of(type, node, ptr)                                          \
    (type *)((char *)(ptr)-offsetof(type, node))

#define list_head(type, node, ptr) container_of(type, node, (ptr)->next)
#define list_next list_head

#define list_for_each_node_unsafe(var, node)                                   \
    for (struct list *var = node->next; var != node; var = var->next)

#define list_for_each(type, var, list, node)                                   \
    for (type *var = list_head(type, node, (list)),                            \
              *__tmp = list_next(type, node, &var->node);                      \
         &var->node != list;                                                   \
         var = __tmp, __tmp = list_next(type, node, &__tmp->node))

static inline void list_insert(struct list *before, struct list *after,
                               struct list *new_node) {
    before->next = new_node;
    new_node->previous = before;

    new_node->next = after;
    after->previous = new_node;
}

static inline void list_append(struct list *head, struct list *new_node) {
    list_insert(head->previous, head, new_node);
}

static inline void list_prepend(struct list *head, struct list *new_node) {
    list_insert(head, head->next, new_node);
}

#define _list_append list_append
#define _list_prepend list_prepend

static inline void list_init(struct list *head) {
    head->next = head;
    head->previous = head;
}

static inline void list_remove_between(struct list *previous,
                                       struct list *next) {
    next->previous = previous;
    previous->next = next;
}

static inline void list_remove(struct list *node) {
    if (node->previous || node->next) {
        list_remove_between(node->previous, node->next);
    }

    node->next = node;
    node->previous = node;
}

static inline bool list_empty(struct list *head) {
    return head->next == head && head->previous == head;
}

static inline bool list_node_null(struct list *node) {
    return node->next == NULL && node->previous == NULL;
}

/*
 * the source list cannot be empty
 */
static inline void list_concat(struct list *dest, struct list *source) {
    struct list *source_head = source->next;
    struct list *source_tail = source->previous;

    dest->previous->next = source_head;
    source_head->previous = dest->previous;

    source_tail->next = dest;
    dest->previous = source_tail;

    list_init(source);
}

static inline struct list *__list_pop_front(struct list *head) {
    struct list *old_head = head->next;
    list_remove_between(head, old_head->next);
    list_init(old_head);
    return old_head;
}

static inline struct list *__list_pop_back(struct list *head) {
    struct list *old_tail = head->previous;
    list_remove_between(old_tail->previous, head);
    list_init(old_tail);
    return old_tail;
}

#define list_pop_front(type, node, ptr)                                        \
    container_of(type, node, __list_pop_front(ptr))

#define list_pop_back(type, node, ptr)                                         \
    container_of(type, node, __list_pop_back(ptr))

static inline size_t list_length(struct list *head) {
    size_t len = 0;
    list_for_each_node_unsafe(_, head) {
        len += 1;
    }
    return len;
}

#endif // _LIST_H_
