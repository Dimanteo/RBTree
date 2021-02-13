#include "RBTree.h"

static const int Left = 0;
static const int Right = 1;

struct RBTree {
        value_t value;
        struct RBTree *children[2];
        struct RBTree *parent;
};

/* Private functions */

int rbt_isempty(struct RBTree *child)
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
        if (!rbt_isempty(left_ch)) {
                retcode |= rbt_destruct(left_ch);
        }
        if (!rbt_isempty(right_ch)) {
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
        if (rbt_isempty(tree->children[child_index])) {
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

int rbt_foreach(struct RBTree *tree, 
                void(*callback)(value_t, struct RBTree*, void*), void *data)
{
        assert(tree);
        assert(callback);
        assert(data);
        if (!tree || !callback || !data) {
                return -1;
        }

        int retcode = 0;
        int err = 0;
        struct RBTree *left_ch = tree->children[Left];
        struct RBTree *right_ch = tree->children[Right];
        if (!rbt_isempty(left_ch)) {
                retcode |= rbt_foreach(left_ch, callback, data);
        }
        callback(rbt_get_val(tree, &err), tree, data);
        if (!rbt_isempty(right_ch)) {
                retcode |= rbt_foreach(right_ch, callback, data);
        }
        if (err) {
                retcode = -1;
        }
        return retcode;
}

struct RBTree *rbt_get_left(const struct RBTree *tree)
{
        assert(tree);
        if (!tree) {
                return NULL;
        }

        return tree->children[Left];
}

struct RBTree *rbt_get_right(const struct RBTree *tree) {
        assert(tree);
        if (!tree) {
                return NULL;
        }

        return tree->children[Right];
}

value_t rbt_get_val(const struct RBTree *tree, int* err) {
        assert(tree);
        if (!tree) {
                if (err != NULL) {
                        *err = 1;
                }
                return 0;
        }

        if (err != NULL) {
                *err = 0;
        }
        return tree->value;
}
