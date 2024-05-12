#pragma once

#include <stddef.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct list_head {
	struct list_head *next;
	struct list_head *previous;
};

#define LIST_INIT(name) \
	{ &(name), &(name) }
#define LIST_HEAD(name) struct list_head name = LIST_INIT(name)

#define container_of(type, node, ptr) \
	(type *)((char *)(ptr)-offsetof(type, node))

#define list_head(list) (list)->next

#define list_for_each(list) \
	for (struct list_head *it = (list)->next; it != (list); it = it->next)

#define list_for_each_safe(list) \
	for (struct list_head *it = (list)->next, *next = it->next; it != (list); \
		 it = next, next = it->next)

static inline void list_insert(struct list_head *before,
	struct list_head *after, struct list_head *new_node) {
	before->next = new_node;
	new_node->previous = before;

	new_node->next = after;
	after->previous = new_node;
}

static inline void list_append(
	struct list_head *head, struct list_head *new_node) {
	list_insert(head->previous, head, new_node);
}

static inline void list_prepend(
	struct list_head *head, struct list_head *new_node) {
	list_insert(head, head->next, new_node);
}

static inline void list_init(struct list_head *head) {
	head->next = head;
	head->previous = head;
}

static inline void list_remove_between(
	struct list_head *previous, struct list_head *next) {
	previous->next = next;
	next->previous = previous;
}

static inline void list_remove(struct list_head *node) {
	if (node->previous || node->next) {
		list_remove_between(node->previous, node->next);
	}
	list_init(node);
}

static inline bool list_empty(struct list_head *head) {
	return head->next == head && head->previous == head;
}

// the source list cannot be empty
static inline void list_concat(
	struct list_head *dest, struct list_head *source) {
	struct list_head *source_head = source->next;
	struct list_head *source_tail = source->previous;

	dest->previous->next = source_head;
	source_head->previous = dest->previous;

	source_tail->next = dest;
	dest->previous = source_tail;

	list_init(source);
}

static inline struct list_head *list_pop_front(struct list_head *head) {
	struct list_head *old_head = head->next;
	list_remove_between(head, old_head->next);
	list_init(old_head);
	return old_head;
}

#define append_to_list(node, list) list_append(list, node)
#define prepend_to_list(node, list) list_prepend(list, node)

END_DECLS
