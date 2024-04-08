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
	node->color = RB_RED;
	rbtree_insert_fixup(tree, node);
}

static void rbtree_insert_fixup(struct rbtree *tree, struct rbnode *node) {
	while (node->parent && node->parent->color == RB_RED) {
		if (node->parent == node->parent->parent->left) {
			struct rbnode *uncle = node->parent->parent->right;
			if (uncle && uncle->color == RB_RED) {
				node->parent->color = RB_BLACK;
				uncle->color = RB_BLACK;
				node->parent->parent->color = RB_RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_left_rotate(tree, node);
				}
				node->parent->color = RB_BLACK;
				node->parent->parent->color = RB_RED;
				rbtree_right_rotate(tree, node->parent->parent);
			}
		} else {
			struct rbnode *uncle = node->parent->parent->left;
			if (uncle && uncle->color == RB_RED) {
				node->parent->color = RB_BLACK;
				uncle->color = RB_BLACK;
				node->parent->parent->color = RB_RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rbtree_right_rotate(tree, node);
				}
				node->parent->color = RB_BLACK;
				node->parent->parent->color = RB_RED;
				rbtree_left_rotate(tree, node->parent->parent);
			}
		}
	}
	tree->root->color = RB_BLACK;
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
	if (node->color == RB_BLACK) {
		rbtree_delete_fixup(tree, child);
	}
	free(node);
}

static void rbtree_delete_fixup(struct rbtree *tree, struct rbnode *node) {
	while (node != tree->root && node->color == RB_BLACK) {
		if (node == node->parent->left) {
			struct rbnode *sibling = node->parent->right;
			if (sibling->color == RB_RED) {
				sibling->color = RB_BLACK;
				node->parent->color = RB_RED;
				rbtree_left_rotate(tree, node->parent);
				sibling = node->parent->right;
			}
			if ((!sibling->left || sibling->left->color == RB_BLACK)
				&& (!sibling->right || sibling->right->color == RB_BLACK)) {
				sibling->color = RB_RED;
				node = node->parent;
			} else {
				if (!sibling->right || sibling->right->color == RB_BLACK) {
					sibling->left->color = RB_BLACK;
					sibling->color = RB_RED;
					rbtree_right_rotate(tree, sibling);
					sibling = node->parent->right;
				}
				sibling->color = node->parent->color;
				node->parent->color = RB_BLACK;
				sibling->right->color = RB_BLACK;
				rbtree_left_rotate(tree, node->parent);
				node = tree->root;
			}
		} else {
			struct rbnode *sibling = node->parent->left;
			if (sibling->color == RB_RED) {
				sibling->color = RB_BLACK;
				node->parent->color = RB_RED;
				rbtree_right_rotate(tree, node->parent);
				sibling = node->parent->left;
			}
			if ((!sibling->left || sibling->left->color == RB_BLACK)
				&& (!sibling->right || sibling->right->color == RB_BLACK)) {
				sibling->color = RB_RED;
				node = node->parent;
			} else {
				if (!sibling->left || sibling->left->color == RB_BLACK) {
					sibling->right->color = RB_BLACK;
					sibling->color = RB_RED;
					rbtree_left_rotate(tree, sibling);
					sibling = node->parent->left;
				}
				sibling->color = node->parent->color;
				node->parent->color = RB_BLACK;
				sibling->left->color = RB_BLACK;
				rbtree_right_rotate(tree, node->parent);
				node = tree->root;
			}
		}
	}
	node->color = RB_BLACK;
}

#define CRED "\e[31m"
#define CBLACK "\e[0m"

void rbtree_visualize(struct rbtree *tree, struct rbnode *current, int depth) {
	if (current->right) {
		rbtree_visualize(tree, current->right, depth + 1);
	}
	printf("%s%d" CBLACK, current->color == RB_RED ? CRED : "", depth);
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

void rbtree_test() {
	struct rbtree tree = { .compare = rbtree_compare_test_int };

	struct rbnode nodes[32] = {};
	for (intptr_t i = 0; i < 32; i++) {
		nodes[i].key = (void *)i;
		nodes[i].value = (void *)(i * 10);
		rbtree_insert(&tree, &nodes[i]);
	}

	for (intptr_t i = 0; i < 32; i++) {
		struct rbnode *node = rbtree_search_le(&tree, (void *)i);
		if (!node) {
			printf(
				"WARN: rbtree was unable to find known-present key %li!\n", i);
			continue;
		} else {
			struct rbnode *pred = rbtree_predecessor(node);
			struct rbnode *succ = rbtree_successor(node);
			if (i > 0 && !pred) {
				printf("WARN: rbtree found no predecessor to %li!\n", i);
			}
			if (i < 31 && !succ) {
				printf("WARN: rbtree found no successor to %li!\n", i);
			}
			if ((intptr_t)node->key * 10 != (intptr_t)node->value) {
				printf("WARN: rbtree key/value mismatch! (%li/%li)\n",
					(intptr_t)node->key, (intptr_t)node->value);
			}
		}
	}
}
