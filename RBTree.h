#ifndef RBTREE_H
#define RBTREE_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

typedef int value_t ;

struct RBTree;

struct RBTree *rbt_init(value_t val);

int rbt_destruct(struct RBTree *tree);

int rbt_insert(struct RBTree *tree, value_t val);

struct RBTree* rbt_find(struct RBTree *tree, value_t val);

int rbt_remove(struct RBTree *node);

int rbt_rmval(struct RBTree *tree, value_t val);

int rbt_foreach(struct RBTree *tree,
                void(*callback)(value_t, struct RBTree*, void*), void *data);

int rbt_isempty(struct RBTree *child);

int rbt_isroot(const struct RBTree *node);

struct RBTree *rbt_get_left(const struct RBTree *tree);

struct RBTree *rbt_get_right(const struct RBTree *tree);

struct RBTree *rbt_get_parent(struct RBTree *tree);

value_t rbt_get_val(const struct RBTree *tree, int* err);

#ifndef NDEBUG
        void rbt_dump(struct RBTree *tree, const char* filename);
#endif

#endif /* RBTREE_H */
