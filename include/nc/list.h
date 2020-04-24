
#pragma once
#ifndef _LIST_H_
#define _LIST_H_

#include <basic.h>
#include <assert.h>
// #include <stdio.h>

struct list {
        struct list *next;
        struct list *prev;
};

typedef struct list list;
typedef struct list list_n;    // REMOVE
typedef struct list list_node; // REMOVE


#define container_of(type, member, pointer) \
        (type *)(((char *)pointer) - offsetof(type, member))

#define list_entry(type, member, pointer) \
        ({ \
            list *__ptr = (pointer); \
            __ptr == NULL ? \
                NULL : \
                container_of(type, member, __ptr); \
        })


static inline
void list_init_if_not_initialized(list *l) {
        if (l->next) {
                assert(l->prev);
                return;
        }
        // printf("list: yolo initializing %p\n", l);
        assert(!l->prev);

        l->next = l;
        l->prev = l;
}

#define list_init list_init_if_not_initialized

static inline
list *list_head(list *l) {
        return l->next;
}

static inline
list *list_next(list *l) {
        return l->next;
}

static inline
bool list_empty(list *l) {
        list_init_if_not_initialized(l);
        return l == list_next(l);
}

#define list_foreach_node(list, node) \
        for (node = list_head(list); \
             node != (list); \
             node = list_next(node))


static inline
void list_remove(list *l) {
        if (!l)  return;
        if (!l->next)  return;

        l->next->prev = l->prev;
        l->prev->next = l->next;

        l->next = NULL;
        l->prev = NULL;
}


#define list_head_entry(type, list, member) \
        list_entry(type, member, list_head(list))

#define list_next_entry(type, list, member) \
        list_entry(type, member, list_next(list))


#define list_foreach(list, var, member) \
        for (var = list_head_entry(typeof(*var), (list), member); \
             var && &var->member != (list); \
             var = list_next_entry(typeof(*var), (&var->member), member)) \

static inline
void _list_append(list *l, list_node *ln) {
        list_init_if_not_initialized(l);

        ln->next = l;
        ln->prev = l->prev;

        l->prev->next = ln;
        l->prev = ln;
}

#define list_append(list, pointer, member) \
        _list_append(list, &(pointer)->member)

static inline
void _list_prepend(list *l, list_node *ln) {
        list_init_if_not_initialized(l);

        ln->prev = l;
        ln->next = l->next;

        l->next->prev = ln;
        l->next = ln;
}

#define list_prepend(list, pointer, member) \
        _list_prepend(list, &(pointer)->member)


static inline
void list_delete(list *before, list *after) {
        before->next = after;
        after->prev = before;
}

static inline
list *list_drop_head(list *l) {
        if (list_empty(l)) return NULL;
        assert(l->next && l->prev);

        list *old = list_head(l);

        // printf("dropping %p\n", old);
        // printf("  l is %p\n", l);
        // printf("  l->next->next is %p\n", l->next->next);

        list_delete(l, l->next->next);

        return old;
}

#define list_pop_front(type, list, member) \
        list_entry(type, member, list_drop_head(list))

static inline
list *list_drop_tail(list *l) {
        if (list_empty(l)) return NULL;
        assert(l->next && l->prev);

        list *old = l->prev;

        l->prev = l->prev->prev;
        l->prev->next = l;

        return old;
}

#define list_pop_back(type, list, member) \
        list_entry(type, member, list_drop_tail(list))


static inline
int list_length(list *l) {
        int count = 0;
        list *node;

        list_foreach_node(l, node) {
                count += 1;
        }
        return count;
}

static inline
void list_insert_after(list_node *ln, list_node *nln) {
        nln->prev = ln;
        nln->next = ln->next;

        ln->next->prev = nln;
        ln->next = nln;
}

static inline
void list_insert_before(list_node *ln, list_node *nln) {
        nln->next = ln;
        nln->prev = ln->prev;

        ln->prev->next = nln;
        ln->prev = nln;
}

#endif // _LIST_H_

