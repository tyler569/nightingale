
#include <ng/basic.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/string.h>
#include <ng/vmm.h>
#include <ds/list.h>

static struct list_n *buffer = NULL;
static struct list_n *free_list = NULL;

void init_global_lists() {
        buffer = vmm_reserve(8 * 1024*1024);
}

static struct list_n *new_free_node() {
        if (!buffer)  panic("lists not set up set");

        struct list_n *res;
        if (free_list) {
                res = free_list;
                free_list = free_list->next;
        } else {
                res = buffer++;
        }
        memset(res, 0, sizeof(*res));
        return res;
}

void *list_head(struct list *l) {
        if (l->head) {
                return l->head->v;
        } else {
                return NULL;
        }
}

void *list_tail(struct list *l) {
        if (l->tail) {
                return l->tail->v;
        } else {
                return NULL;
        }
}

int list_prepend(struct list *l, void *v) {
        struct list_n *node = new_free_node();
        node->next = l->head;
        if (l->head) {
                l->head->prev = node;
        }
        if (!l->tail) {
                l->tail = node;
        }
        node->v = v;
        l->head = node;

        return 0;
}

int list_append(struct list *l, void *v) {
        struct list_n *node = new_free_node();
        node->prev = l->tail;
        if (l->tail) {
                l->tail->next = node;
        }
        if (!l->head) {
                l->head = node;
        }
        node->v = v;
        l->tail = node;

        return 0;
}

void list_foreach(struct list *l, void (*fn)(void *)) {
        struct list_n *node = l->head;
        for (; node; node = node->next) {
                fn(node->v);
        }
}

static void add_node_to_free_list(struct list_n *node) {
        node->next = free_list;
        // printf("free_list is %p\n", free_list);
        if (free_list)
                free_list->prev = node;
        node->v = NULL;
        free_list = node;
}


void list_free(struct list *l) {
        struct list_n *node = l->head;
        struct list_n *next = node;
        while (node) {
                next = node->next;
                add_node_to_free_list(node);
                node = next;
        }
}

void *list_pop_front(struct list *l) {
        struct list_n *node = l->head;
        if (!node)  return NULL;
        void *res = node->v;
        
        l->head = node->next;
        if (l->head) {
                l->head->prev = NULL;
        } else {
                l->tail = NULL;
        }
        
        add_node_to_free_list(node);
        return res;
}

void *list_pop_back(struct list *l) {
        struct list_n *node = l->tail;
        if (!node)  return NULL;
        void *res = node->v;
        
        l->tail = node->prev;
        if (l->tail) {
                l->tail->next = NULL;
        } else {
                l->head = NULL;
        }
        
        add_node_to_free_list(node);
        return res;
}

