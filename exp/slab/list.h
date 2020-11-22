
#pragma once
#ifndef NG_LIST_H
#define NG_LIST_H

#ifdef __ngk__
#include <basic.h>
#include <stdio.h>
// #include <nc/stdio.h>
#else
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#endif

struct list {
    struct list *next;
    struct list *prev;
};

typedef struct list list;
typedef struct list list_n;    // REMOVE
typedef struct list list_node; // REMOVE


#define container_of(type, member, pointer)                                    \
    (type *) (((char *) pointer) - offsetof(type, member))

#define list_entry(type, member, pointer)                                      \
    ({                                                                         \
        list *__ptr = (pointer);                                               \
        __ptr == NULL ? NULL : container_of(type, member, __ptr);              \
    })


static inline list *list_head(list *l) {
    return l->next;
}

static inline list *list_next(list *l) {
    return l->next;
}

static inline bool list_empty(list *l) {
    return l == list_next(l);
}

#define list_foreach_node(list, node)                                          \
    for (node = list_head(list); node != (list); node = list_next(node))


static inline void list_remove(list *l) {
    if (!l) return;
    if (!l->next) return;

    l->next->prev = l->prev;
    l->prev->next = l->next;

    l->next = NULL;
    l->prev = NULL;
}


#define list_head_entry(type, list, member)                                    \
    list_entry(type, member, list_head(list))

#define list_next_entry(type, list, member)                                    \
    list_entry(type, member, list_next(list))


#define list_foreach(list, var, member)                                        \
    for (var = list_head_entry(typeof(*var), (list), member);                  \
         var && &var->member != (list);                                        \
         var = list_next_entry(typeof(*var), (&var->member), member))

static inline void _list_append(list *l, list_node *ln) {
    ln->next = l;
    ln->prev = l->prev;

    l->prev->next = ln;
    l->prev = ln;
}

#define list_append(list, pointer, member)                                     \
    _list_append(list, &(pointer)->member)

static inline void _list_prepend(list *l, list_node *ln) {
    ln->prev = l;
    ln->next = l->next;

    l->next->prev = ln;
    l->next = ln;
}

#define list_prepend(list, pointer, member)                                    \
    _list_prepend(list, &(pointer)->member)


static inline void list_delete(list *before, list *after) {
    before->next = after;
    after->prev = before;
}

static inline list *list_drop_head(list *l) {
    if (list_empty(l)) { return NULL; }

    list *old = list_head(l);

    list_delete(l, l->next->next);

    return old;
}

#define list_pop_front(type, list, member)                                     \
    list_entry(type, member, list_drop_head(list))

static inline list *list_drop_tail(list *l) {
    if (list_empty(l)) return NULL;

    list *old = l->prev;

    l->prev = l->prev->prev;
    l->prev->next = l;

    return old;
}

#define list_pop_back(type, list, member)                                      \
    list_entry(type, member, list_drop_tail(list))


static inline void list_init(list *l) {
    l->next = l;
    l->prev = l;
}

static inline int list_length(list *l) {
    int count = 0;
    list *node;

    list_foreach_node(l, node) {
        count += 1;
    }
    return count;
}

#endif // NG_LIST_H
