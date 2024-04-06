#pragma once

enum rbcolor { BLACK, RED };

struct rbnode {
	void *key;
	void *value;

	struct rbnode *left;
	struct rbnode *right;
	struct rbnode *parent;
	enum rbcolor color;
};

struct rbtree {
	struct rbnode *root;
	int (*compare)(void *, void *);
};

void rbtree_insert(struct rbtree *tree, struct rbnode *node);
void rbtree_delete(struct rbtree *tree, void *key);
struct rbnode *rbtree_search(struct rbtree *tree, void *key);
struct rbnode *rbtree_min(struct rbtree *tree);
struct rbnode *rbtree_max(struct rbtree *tree);
struct rbnode *rbtree_successor(struct rbnode *node);
struct rbnode *rbtree_predecessor(struct rbnode *node);
struct rbnode *rbtree_search_ge(struct rbtree *tree, void *key);
struct rbnode *rbtree_search_le(struct rbtree *tree, void *key);
