#include "RBTree.h"

static const int Left = 0;
static const int Right = 1;

struct RBTree {
        value_t value;
        struct RBTree *children[2];
        struct RBTree *parent;
};

/* Private functions */

int is_empty_ch(struct RBTree *child)
{
        if (child == NULL) {
                return 1;
        } else {
                return 0;
        }
}

/* Public interfaces */

struct RBTree *rbt_init(value_t val)
{
        struct RBTree *tree = malloc(sizeof(struct RBTree));
        if (tree == NULL) {
                return NULL;
        }
        tree->parent = NULL;
        tree->children[Left] = NULL;
        tree->children[Right] = NULL;
        tree->value = val;

        return tree;
}

int rbt_destruct(struct RBTree *tree) 
{
        assert(tree);
        if (!tree) {
                return -1;
        }

        int retcode = 0;
        struct RBTree *left_ch = tree->children[Left];
        struct RBTree *right_ch = tree->children[Right];
        if (!is_empty_ch(left_ch)) {
                retcode |= rbt_destruct(left_ch);
        }
        if (!is_empty_ch(right_ch)) {
                retcode |= rbt_destruct(right_ch);
        }
        free(tree);
        return retcode;
}

int rbt_insert(struct RBTree *tree, value_t val)
{
        assert(tree);
        if (!tree)
                return -1;

        int child_index;

        if (val > tree->value) {
                child_index = Right;
        } else if (val < tree->value) {
                child_index = Left;
        } else {
                return 0;
        }
        if (is_empty_ch(tree->children[child_index])) {
                struct RBTree *tmp = rbt_init(val);
                if (tmp == NULL) {
                        return -1;
                }
                tree->children[child_index] = tmp;
        } else {
                int retcode = rbt_insert(tree->children[child_index], val);
                return retcode;
        }

        return 0;
}
