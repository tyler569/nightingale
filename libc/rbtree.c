#include <rbtree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vec.h>

static void rbtree_insert_fixup(struct rbtree *tree, struct rbnode *node);
static void rbtree_delete_fixup(struct rbtree *tree, struct rbnode *node);
static void rbtree_left_rotate(struct rbtree *tree, struct rbnode *node);
static void rbtree_right_rotate(struct rbtree *tree, struct rbnode *node);

void rbtree_insert(struct rbtree *tree, struct rbnode *node) {
	struct rbnode *parent = NULL;
	struct rbnode *current = tree->root;
	while (current) {
		parent = current;
		if (tree->compare(node->key, current->key) < 0) {
			current = current->left;
		} else {
			current = current->right;
		}
	}
	node->parent = parent;
	if (!parent) {
		tree->root = node;
	} else if (tree->compare(node->key, parent->key) < 0) {
		parent->left = node;
	} else {
		parent->right = node;
	}
	node->left = NULL;
	node->right = NULL;
	node->color = RED;
	rbtree_insert_fixup(tree, node);
}

static void rbtree_insert_fixup(struct rbtree *tree, struct rbnode *node) {
	while (node->parent && node->parent->color == RED) {
		if (node->parent == node->parent->parent->left) {
			struct rbnode *uncle = node->parent->parent->right;
			if (uncle && uncle->color == RED) {
				node->parent->color = BLACK;
				uncle->color = BLACK;
				node->parent->parent->color = RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_left_rotate(tree, node);
				}
				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				rbtree_right_rotate(tree, node->parent->parent);
			}
		} else {
			struct rbnode *uncle = node->parent->parent->left;
			if (uncle && uncle->color == RED) {
				node->parent->color = BLACK;
				uncle->color = BLACK;
				node->parent->parent->color = RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rbtree_right_rotate(tree, node);
				}
				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				rbtree_left_rotate(tree, node->parent->parent);
			}
		}
	}
	tree->root->color = BLACK;
}

static void rbtree_left_rotate(struct rbtree *tree, struct rbnode *node) {
	struct rbnode *right = node->right;
	node->right = right->left;
	if (right->left) {
		right->left->parent = node;
	}
	right->parent = node->parent;
	if (!node->parent) {
		tree->root = right;
	} else if (node == node->parent->left) {
		node->parent->left = right;
	} else {
		node->parent->right = right;
	}
	right->left = node;
	node->parent = right;
}

static void rbtree_right_rotate(struct rbtree *tree, struct rbnode *node) {
	struct rbnode *left = node->left;
	node->left = left->right;
	if (left->right) {
		left->right->parent = node;
	}
	left->parent = node->parent;
	if (!node->parent) {
		tree->root = left;
	} else if (node == node->parent->right) {
		node->parent->right = left;
	} else {
		node->parent->left = left;
	}
	left->right = node;
	node->parent = left;
}

struct rbnode *rbtree_search(struct rbtree *tree, void *key) {
	struct rbnode *current = tree->root;
	while (current) {
		int cmp = tree->compare(key, current->key);
		if (cmp == 0) {
			return current;
		} else if (cmp < 0) {
			current = current->left;
		} else {
			current = current->right;
		}
	}
	return NULL;
}

struct rbnode *rbtree_min(struct rbtree *tree) {
	struct rbnode *current = tree->root;
	while (current && current->left) {
		current = current->left;
	}
	return current;
}

struct rbnode *rbtree_max(struct rbtree *tree) {
	struct rbnode *current = tree->root;
	while (current && current->right) {
		current = current->right;
	}
	return current;
}

struct rbnode *rbtree_successor(struct rbnode *node) {
	if (node->right) {
		node = node->right;
		while (node->left) {
			node = node->left;
		}
		return node;
	}
	struct rbnode *parent = node->parent;
	while (parent && node == parent->right) {
		node = parent;
		parent = parent->parent;
	}
	return parent;
}

struct rbnode *rbtree_predecessor(struct rbnode *node) {
	if (node->left) {
		node = node->left;
		while (node->right) {
			node = node->right;
		}
		return node;
	}
	struct rbnode *parent = node->parent;
	while (parent && node == parent->left) {
		node = parent;
		parent = parent->parent;
	}
	return parent;
}

struct rbnode *rbtree_search_ge(struct rbtree *tree, void *key) {
	struct rbnode *current = tree->root;
	struct rbnode *ge = NULL;
	while (current) {
		int cmp = tree->compare(key, current->key);
		if (cmp == 0) {
			return current;
		} else if (cmp < 0) {
			ge = current;
			current = current->left;
		} else {
			current = current->right;
		}
	}
	return ge;
}

struct rbnode *rbtree_search_le(struct rbtree *tree, void *key) {
	struct rbnode *current = tree->root;
	struct rbnode *le = NULL;
	while (current) {
		int cmp = tree->compare(key, current->key);
		if (cmp == 0) {
			return current;
		} else if (cmp < 0) {
			current = current->left;
		} else {
			le = current;
			current = current->right;
		}
	}
	return le;
}

void rbtree_delete(struct rbtree *tree, void *key) {
	struct rbnode *node = tree->root;
	while (node) {
		int cmp = tree->compare(key, node->key);
		if (cmp == 0) {
			break;
		} else if (cmp < 0) {
			node = node->left;
		} else {
			node = node->right;
		}
	}
	if (!node) {
		return;
	}

	struct rbnode *child;
	if (!node->left) {
		child = node->right;
	} else if (!node->right) {
		child = node->left;
	} else {
		struct rbnode *successor = node->right;
		while (successor->left) {
			successor = successor->left;
		}
		child = successor->right;
		if (successor->parent != node) {
			successor->parent->left = child;
			if (child) {
				child->parent = successor->parent;
			}
			successor->right = node->right;
			node->right->parent = successor;
		}
		successor->left = node->left;
		node->left->parent = successor;
	}
	if (node->parent) {
		if (node == node->parent->left) {
			node->parent->left = child;
		} else {
			node->parent->right = child;
		}
	} else {
		tree->root = child;
	}
	if (child) {
		child->parent = node->parent;
	}
	if (node->color == BLACK) {
		rbtree_delete_fixup(tree, child);
	}
	free(node);
}

static void rbtree_delete_fixup(struct rbtree *tree, struct rbnode *node) {
	while (node != tree->root && node->color == BLACK) {
		if (node == node->parent->left) {
			struct rbnode *sibling = node->parent->right;
			if (sibling->color == RED) {
				sibling->color = BLACK;
				node->parent->color = RED;
				rbtree_left_rotate(tree, node->parent);
				sibling = node->parent->right;
			}
			if ((!sibling->left || sibling->left->color == BLACK)
				&& (!sibling->right || sibling->right->color == BLACK)) {
				sibling->color = RED;
				node = node->parent;
			} else {
				if (!sibling->right || sibling->right->color == BLACK) {
					sibling->left->color = BLACK;
					sibling->color = RED;
					rbtree_right_rotate(tree, sibling);
					sibling = node->parent->right;
				}
				sibling->color = node->parent->color;
				node->parent->color = BLACK;
				sibling->right->color = BLACK;
				rbtree_left_rotate(tree, node->parent);
				node = tree->root;
			}
		} else {
			struct rbnode *sibling = node->parent->left;
			if (sibling->color == RED) {
				sibling->color = BLACK;
				node->parent->color = RED;
				rbtree_right_rotate(tree, node->parent);
				sibling = node->parent->left;
			}
			if ((!sibling->left || sibling->left->color == BLACK)
				&& (!sibling->right || sibling->right->color == BLACK)) {
				sibling->color = RED;
				node = node->parent;
			} else {
				if (!sibling->left || sibling->left->color == BLACK) {
					sibling->right->color = BLACK;
					sibling->color = RED;
					rbtree_left_rotate(tree, sibling);
					sibling = node->parent->left;
				}
				sibling->color = node->parent->color;
				node->parent->color = BLACK;
				sibling->left->color = BLACK;
				rbtree_right_rotate(tree, node->parent);
				node = tree->root;
			}
		}
	}
	node->color = BLACK;
}

#define CRED "\e[31m"
#define CBLACK "\e[0m"

void rbtree_visualize(struct rbtree *tree, struct rbnode *current, int depth) {
	if (current->right) {
		rbtree_visualize(tree, current->right, depth + 1);
	}
	printf("%s%d" CBLACK, current->color == RED ? CRED : "", depth);
	if (current->left) {
		rbtree_visualize(tree, current->left, depth + 1);
	}
	if (depth == 0) {
		printf("\n");
	}
}

int rbtree_compare_test_int(void *a, void *b) {
	return (int)((intptr_t)a - (intptr_t)b);
}

int rbtree_compare_test_string(void *a, void *b) {
	return strcmp((const char *)a, (const char *)b);
}

int rbtree_random() {
	static unsigned long seed = 0;
	seed = seed * 1103515245 + 12345;
	return (unsigned int)(seed / 65536) % 32768;
}

void rbtree_test() {
	struct rbtree tree = { .compare = rbtree_compare_test_string };

	struct rbnode nodes[32] = {};
	for (int i = 0; i < 32; i++) {
		char *key = calloc(16, 1);
		snprintf(key, 16, "key%d", rbtree_random());

		nodes[i].key = key;
		nodes[i].value = (void *)(intptr_t)(i * 10);
		rbtree_insert(&tree, &nodes[i]);

		printf("insert '%d': '%s' '%d' ", i, (char *)nodes[i].key,
			(int)(intptr_t)nodes[i].value);

		rbtree_visualize(&tree, tree.root, 0);
	}

	char buf[16];
	for (int i = 0; i < 32; i++) {
		snprintf(buf, 16, "key%d1", i);
		struct rbnode *node = rbtree_search_le(&tree, buf);
		if (node) {
			printf("search '%s': '%s' '%d'\n", buf, (char *)node->key,
				(int)(intptr_t)node->value);

			struct rbnode *succ = rbtree_successor(node);
			struct rbnode *pred = rbtree_predecessor(node);
			printf("%d %s -> %s -> %d %s\n", succ ? (int)(intptr_t)succ->value : -1,
				succ ? (char *)succ->key : "NULL", (char *)node->key,
				pred ? (int)(intptr_t)pred->value : -1, pred ? (char *)pred->key : "NULL");
		} else {
			printf("search '%s' failed\n", buf);
		}
	}
}
