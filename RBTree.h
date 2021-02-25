#ifndef RBTREE_H
#define RBTREE_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

typedef int value_t ;

struct RBTree;

struct RBTree *rbt_init();

int rbt_destruct(struct RBTree *tree);

int rbt_insert(struct RBTree *tree, value_t val);

int rbt_contains(const struct RBTree *tree, value_t val);

int rbt_remove(struct RBTree *tree, value_t val);

int rbt_foreach(struct RBTree *tree,
                void(*callback)(value_t, struct RBTree*, void*), void *data);

#ifndef NDEBUG
        void rbt_dump(struct RBTree *tree, const char* filename);
#endif

#endif /* RBTREE_H */
