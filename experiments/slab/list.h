#pragma once

#include <stddef.h>

struct list_node {
	struct list_node *prev, *next;
};
struct list_head {
	struct list_node n;
};

static inline void list_init(struct list_head *h) {
	h->n.next = h->n.prev = &h->n;
}

#define list_init_static(head) { .n.prev = &(head).n, .n.next = &(head).n }

static inline int list_empty(const struct list_head *h) {
	return h->n.next == &h->n;
}

static inline void __list_insert(struct list_node *pos, struct list_node *n) {
	n->next = pos->next;
	n->prev = pos;
	pos->next->prev = n;
	pos->next = n;
}

static inline void list_push_back(struct list_head *h, struct list_node *n) {
	__list_insert(h->n.prev, n);
}

static inline void list_push_front(struct list_head *h, struct list_node *n) {
	__list_insert(&h->n, n);
}

static inline void list_remove(struct list_node *n) {
	n->prev->next = n->next;
	n->next->prev = n->prev;
	n->next = n->prev = nullptr;
}

#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

#define list_entry(node_ptr, type, member) \
	container_of((node_ptr), type, member)

#define list_for_each(head, it) \
	for (struct list_node * (it) = (head)->n.next; (it) != &(head)->n; \
		(it) = (it)->next)

#define list_for_each_safe(head, it, tmp) \
	for (struct list_node * (it) = (head)->n.next, *(tmp) = (it)->next; \
		(it) != &(head)->n; (it) = (tmp), (tmp) = (it)->next)
