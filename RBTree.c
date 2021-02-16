#include "RBTree.h"

enum Color {BLACK, RED};

static const unsigned int NLeaves = 2u;
static const int Left = 0;
static const int Right = 1;

struct RBTree {
        value_t value;
        enum Color color;
        struct RBTree *children[2];
        struct RBTree *parent;
};

static enum Color get_color(struct RBTree *tree);

static int balance(struct RBTree *node);

static void rotate_left(struct RBTree *node);

static void rotate_right(struct RBTree *node);

int rbt_isempty(struct RBTree *leaf)
{
        if (leaf == NULL) {
                return 1;
        } else {
                return 0;
        }
}

int rbt_isroot(const struct RBTree *node)
{
        if (node == NULL || node->parent != NULL) {
                return 0;
        } else {
                return 1;
        }
}

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
        tree->color = BLACK;

        return tree;
}

int rbt_destruct(struct RBTree *tree) 
{
        assert(tree);
        if (tree == NULL) {
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
        if (tree == NULL) {
                return -1;
        }

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
                tmp->parent = tree;
                tmp->color = RED;
                balance(tmp);
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

struct RBTree* rbt_find(const struct RBTree *tree, value_t val)
{
        assert(tree);
        if (tree == NULL) {
                return NULL;
        }

        int err = 0;
        value_t cur_val = rbt_get_val(tree, &err);
        if (err) {
                return NULL;
        }
        if (val == cur_val) {
                return tree;
        }
        struct RBTree *ret_node = NULL;
        if (val < cur_val) {
                struct RBTree *left_ch = rbt_get_left(tree);
                if (left_ch != NULL) {
                        ret_node = rbt_find(left_ch, val);
                }
        } else {
                struct RBTree *right_ch = rbt_get_right(tree);
                if (right_ch != NULL) {
                        ret_node = rbt_find(right_ch, val);
                }
        }

        return ret_node;
}

int rbt_remove(struct RBTree *tree) 
{
        assert(tree);
        if (tree == NULL) {
                return -1;
        }

        if (rbt_isroot(tree)) {
                struct RBTree *right_ch = rbt_get_right(tree);
                struct RBTree *left_ch = rbt_get_left(tree);
                
        } else {
                struct RBTree *parent = rbt_get_parent(tree);
        }

        return 0;
}

struct RBTree *rbt_get_left(const struct RBTree *tree)
{
        assert(tree);
        if (tree == NULL) {
                return NULL;
        }

        return tree->children[Left];
}

struct RBTree *rbt_get_right(const struct RBTree *tree) 
{
        assert(tree);
        if (tree == NULL) {
                return NULL;
        }

        return tree->children[Right];
}

struct RBTree *rbt_get_parent(const struct RBTree *tree)
{
        assert(tree);
        if (tree == NULL) {
                return NULL;
        }

        if (tree->parent == NULL) {
                return tree;
        }

        return tree->parent;
}

value_t rbt_get_val(const struct RBTree *tree, int* err) 
{
        assert(tree);
        if (tree == NULL) {
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

static enum Color get_color(struct RBTree *tree)
{
        assert(tree);
        if (rbt_isempty(tree) || tree == NULL) {
                return BLACK;
        }

        return tree->color;
}

static void rotate_left(struct RBTree *node)
{
        struct RBTree *pivot = rbt_get_right(node);
        assert(pivot);

        if (!rbt_isroot(node)) {
                struct RBTree *parent = rbt_get_parent(node);
                assert(parent);

                if (parent->children[Left] == node) {
                        parent->children[Left] = pivot;
                } else {
                        parent->children[Right] = pivot;
                }
        }

        pivot->parent = node->parent;
        node->parent = pivot;
        node->children[Right] = rbt_get_left(pivot);
        pivot->children[Left] = node;
}

static void rotate_right(struct RBTree *node)
{
        struct RBTree *pivot = rbt_get_left(node);
        assert(pivot);

        if (!rbt_isroot(node)) {
                struct RBTree *parent = rbt_get_parent(node);
                assert(parent);

                if (parent->children[Left] == node) {
                        parent->children[Left] = pivot;
                } else {
                        parent->children[Right] = pivot;
                }
        }

        pivot->parent = node->parent;
        node->parent = pivot;
        node->children[Left] = rbt_get_right(pivot);
        pivot->children[Right] = node;
}

static int balance(struct RBTree *node)
{
        assert(node);

        struct RBTree *parent = NULL;
        struct RBTree *uncle = NULL;
        struct RBTree *granddad = NULL;

        // case 1
        if (rbt_isroot(node)) {
                node->color = BLACK;
                return 0;
        }

        parent = rbt_get_parent(node);
        if (parent == NULL) {
                return -1;
        }

        // case 2
        if (get_color(parent) == BLACK) {
                return 0;
        }

        granddad = rbt_get_parent(parent);
        if (granddad == NULL) {
                return -1;
        }

        /* looking for uncle
         * and memorizing side of parent for case 4 */
        int par_isleft = 0;
        uncle = rbt_get_left(granddad);
        if (uncle == parent) {
                uncle = rbt_get_right(granddad);
                par_isleft = 1;
        }

        /* case 3
         * get_color doesn't throw errors
         * at this point and further parent.color is RED,
         * because otherwise it would be case 2 */
        if (get_color(uncle) == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                granddad->color = RED;
                return balance(granddad);;
        }

        /* case 4
         * if uncle.color == BLACK */
        if (par_isleft) {
                if (rbt_get_right(parent) == node) {
                        rotate_left(parent);
                        node->color = BLACK;
                        granddad->color = RED;
                        rotate_right(granddad);
                }
        } else {
                if (rbt_get_left(parent) == node) {
                        rotate_right(parent);
                        node->color = BLACK;
                        granddad->color = RED;
                        rotate_left(granddad);
                }
        }

        return 0;
}
