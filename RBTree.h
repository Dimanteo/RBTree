#ifndef RBTREE_H
#define RBTREE_H

#include <assert.h>
#include <stdlib.h>

typedef int value_t ;

struct RBTree;

struct RBTree *rbt_init(value_t val);

int rbt_destruct(struct RBTree *tree);

int rbt_insert(struct RBTree *tree, value_t val);

void rbt_find(struct RBTree *tree, value_t val);

void rbt_remove(struct RBTree *tree, value_t val);

void rbt_foreach(struct RBTree *tree, 
                int(*callback)(value_t*, struct RBTree*, void*), void *data);

int is_empty_ch(struct RBTree *child);


#endif /* RBTREE_H */
