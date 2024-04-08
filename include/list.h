#pragma once
#ifndef _LIST_H_
#define _LIST_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct list {
	struct list *next;
	struct list *previous;
};

typedef struct list list;
typedef struct list list_node;

#define LIST_INIT(name) \
	{ &(name), &(name) }
#define LIST_DEFINE(name) list name = LIST_INIT(name)

#define container_of(type, node, ptr) \
	(type *)((char *)(ptr)-offsetof(type, node))

#define list_head(list) (list)->next

#define list_for_each(list) \
	for (list_node *it = (list)->next; it != (list); it = it->next)

#define list_for_each_safe(list) \
	for (list_node *it = (list)->next, *next = it->next; it != (list); \
		 it = next, next = it->next)

static inline void list_insert(
	list_node *before, list_node *after, list_node *new_node) {
	before->next = new_node;
	new_node->previous = before;

	new_node->next = after;
	after->previous = new_node;
}

static inline void list_append(list_node *head, list_node *new_node) {
	list_insert(head->previous, head, new_node);
}

static inline void list_prepend(list_node *head, list_node *new_node) {
	list_insert(head, head->next, new_node);
}

static inline void list_init(list_node *head) {
	head->next = head;
	head->previous = head;
}

static inline void list_remove_between(list_node *previous, list_node *next) {
	previous->next = next;
	next->previous = previous;
}

static inline void list_remove(list_node *node) {
	if (node->previous || node->next) {
		list_remove_between(node->previous, node->next);
	}
	list_init(node);
}

static inline bool list_empty(list_node *head) {
	return head->next == head && head->previous == head;
}

// the source list cannot be empty
static inline void list_concat(list_node *dest, list_node *source) {
	list_node *source_head = source->next;
	list_node *source_tail = source->previous;

	dest->previous->next = source_head;
	source_head->previous = dest->previous;

	source_tail->next = dest;
	dest->previous = source_tail;

	list_init(source);
}

static inline list_node *list_pop_front(list_node *head) {
	list_node *old_head = head->next;
	list_remove_between(head, old_head->next);
	list_init(old_head);
	return old_head;
}

END_DECLS

#endif // _LIST_H_
