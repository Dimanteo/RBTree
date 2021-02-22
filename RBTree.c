#include "RBTree.h"

enum Color {BLACK, RED};
enum Side {LEFT = 0, RIGHT = 1, ROOT = -1};

struct RBTree {
        value_t value;
        enum Color color;
        struct RBTree *children[2];
        struct RBTree *parent;
};

static enum Color get_color(struct RBTree *tree);

static void set_color(struct RBTree *node, enum Color clr);

static void swap_color(struct RBTree *node1, struct RBTree *node2);

static enum Side get_side(struct RBTree *node);

static int insert_balance(struct RBTree *node);

static void remove_balance(struct RBTree *node);

static void rotate_left(struct RBTree *node);

static void rotate_right(struct RBTree *node);

static void set_child(struct RBTree *parent, struct RBTree *child, enum Side side);

static struct RBTree *get_rightmost(struct RBTree *node);

static void destruct(struct RBTree *node);

static int subtree_destruct(struct RBTree *node);

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
        tree->children[LEFT] = NULL;
        tree->children[RIGHT] = NULL;
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
        assert(tree->parent == NULL);
        if (!rbt_isroot(tree)) {
                return -1;
        }

        return subtree_destruct(tree);
}

int rbt_insert(struct RBTree *tree, value_t val)
{
        assert(tree);
        if (tree == NULL) {
                return -1;
        }

        int child_index;

        if (val > tree->value) {
                child_index = RIGHT;
        } else if (val < tree->value) {
                child_index = LEFT;
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
                insert_balance(tmp);
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
        struct RBTree *left_ch = tree->children[LEFT];
        struct RBTree *right_ch = tree->children[RIGHT];
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

struct RBTree* rbt_find(struct RBTree *tree, value_t val)
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

int rbt_rmval(struct RBTree *tree, value_t val)
{
        struct RBTree *node = rbt_find(tree, val);
        if (node == NULL) {
                return -1;
        }
        return rbt_remove(node);
}

int rbt_remove(struct RBTree *node) 
{
        assert(node);
        if (node == NULL) {
                return -1;
        }
        
        /* Reducing to case of deleting node with at least one
         * empty child. Because maximum in left subtree
         * can't have right child. */
        if (!rbt_isempty(rbt_get_right(node)) && 
                        !rbt_isempty(rbt_get_left(node))) {
                struct RBTree *rmost = get_rightmost(rbt_get_left(node));
                node->value = rmost->value;
                node = rmost;
        }

        if (get_color(node) == RED) {
                /* This can only happen if 
                 * both children are empty. */
                destruct(node);
        } else {
                struct RBTree *child = rbt_get_left(node);
                if (rbt_isempty(child)) {
                        child = rbt_get_right(node);
                }

                if (get_color(child) == RED) {
                        set_color(child, BLACK);
                        enum Side sd = get_side(node);
                        if (sd != ROOT) {
                                struct RBTree *parent = rbt_get_parent(node);
                                set_child(parent, child, sd);
                        }
                        destruct(node);
                } else {
                        // This can happen only if both children are empty
                        // FIXME
                        remove_balance(node);
                        destruct(node);
                }
        }

        return 0;
}

static void remove_balance(struct RBTree *node)
{
        assert(node);
        /* node color must be BLACK,
         * because RED case was handled in caller function
         * and doesn't require to rebalance tree */
        assert(node->color == BLACK);

        // case 1
        if (rbt_isroot(node)) {
                return;
        }

        struct RBTree *parent = rbt_get_parent(node);
        struct RBTree *sibling = NULL;
        int sib_isright = 0;
        if (rbt_get_left(parent) == node) {
                sibling = rbt_get_right(parent);
                sib_isright = 1;
        } else {
                sibling = rbt_get_left(parent);
        }
        struct RBTree *sib_l = rbt_get_left(sibling);
        struct RBTree *sib_r = rbt_get_right(sibling);

        // case 2
        if (get_color(sibling) == RED) {
                set_color(sibling, BLACK);
                set_color(parent, RED);
                if (sib_isright) {
                        rotate_left(parent);
                        sibling = sib_l;
                } else {
                        rotate_right(parent);
                        sibling = sib_r;
                }
                sib_l = rbt_get_left(sibling);
                sib_r = rbt_get_right(sibling);
        }
        
        if (get_color(parent) == BLACK && get_color(sibling) == BLACK &&
                get_color(sib_r) == BLACK && get_color(sib_l) == BLACK) {
                // case 3
                // node, parent, sibling and sibling's children are BLACK
                set_color(sibling, RED);
                remove_balance(parent);
        } else if (get_color(sib_r) == BLACK && get_color(sib_l) == BLACK &&
                get_color(sibling) == BLACK && get_color(parent) == RED) {
                //case 4
                /* node, sibling and sibling children are black,
                 * but parent is red */
                set_color(parent, BLACK);
                set_color(sibling, RED);
                return;

        /* the following statements just force the red 
         * to be on the left of the left of the parent, 
         * or right of the right, so case 6 will rotate correctly. */
        } else if (get_color(sib_l) == RED && get_color(sib_l) == BLACK) {
                // case 5 left is red
                rotate_right(sibling);
                set_color(sib_l, BLACK);
                set_color(sibling, RED);
                sibling = sib_l;
        } else if (get_color(sib_r) == RED && get_color(sib_l) == BLACK) {
                // case 5 right is red
                rotate_left(sibling);
                set_color(sib_r, BLACK);
                set_color(sibling, RED);
                sibling = sib_r;
        }

        //case 6
        sib_l = rbt_get_left(sibling);
        sib_r = rbt_get_right(sibling);
        swap_color(sibling, parent);
        if (node == rbt_get_left(parent)) {
                set_color(sib_r, BLACK);
                rotate_left(parent);
        } else {
                set_color(sib_l, BLACK);
                rotate_right(parent);
        }
}

struct RBTree *rbt_get_left(const struct RBTree *tree)
{
        assert(tree);
        if (tree == NULL) {
                return NULL;
        }

        return tree->children[LEFT];
}

struct RBTree *rbt_get_right(const struct RBTree *tree) 
{
        assert(tree);
        if (tree == NULL) {
                return NULL;
        }

        return tree->children[RIGHT];
}

struct RBTree *rbt_get_parent(struct RBTree *tree)
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
        if (rbt_isempty(tree) || tree == NULL) {
                return BLACK;
        }

        return tree->color;
}

static void set_color(struct RBTree *node, enum Color clr)
{
        assert(node);
        node->color = clr;
}

static void swap_color(struct RBTree *node1, struct RBTree *node2)
{
        enum Color tmp = get_color(node2);
        set_color(node2, get_color(node1));
        set_color(node1, tmp);
}

static void rotate_left(struct RBTree *node)
{
        struct RBTree *pivot = rbt_get_right(node);
        assert(pivot);

        if (!rbt_isroot(node)) {
                struct RBTree *parent = rbt_get_parent(node);
                assert(parent);

                if (parent->children[LEFT] == node) {
                        parent->children[LEFT] = pivot;
                } else {
                        parent->children[RIGHT] = pivot;
                }
        }

        pivot->parent = node->parent;
        node->parent = pivot;
        node->children[RIGHT] = rbt_get_left(pivot);
        pivot->children[LEFT] = node;
}

static void rotate_right(struct RBTree *node)
{
        struct RBTree *pivot = rbt_get_left(node);
        assert(pivot);

        if (!rbt_isroot(node)) {
                struct RBTree *parent = rbt_get_parent(node);
                assert(parent);

                if (parent->children[LEFT] == node) {
                        parent->children[LEFT] = pivot;
                } else {
                        parent->children[RIGHT] = pivot;
                }
        }

        pivot->parent = node->parent;
        node->parent = pivot;
        node->children[LEFT] = rbt_get_right(pivot);
        pivot->children[RIGHT] = node;
}

static int insert_balance(struct RBTree *node)
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
                return insert_balance(granddad);;
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

static void set_child(struct RBTree *parent, struct RBTree *child, enum Side side)
{
        assert(side != ROOT);
        parent->children[side] = child;
        if (child != NULL) {
                child->parent = parent;
        }
}

static struct RBTree *get_rightmost(struct RBTree *node)
{
        struct RBTree *right_ch = rbt_get_right(node);
        while (!rbt_isempty(right_ch)) {
                node = right_ch;
                right_ch = rbt_get_right(node);
        }
        return node;
}

static void destruct(struct RBTree *node)
{
        assert(node);
        enum Side side = get_side(node);
        if (side != ROOT) {
                struct RBTree *parent = rbt_get_parent(node);
                set_child(parent, NULL, side);
        }
        free(node);
}

static int subtree_destruct(struct RBTree *node)
{
        assert(node);

        int retcode = 0;
        struct RBTree *left_ch = rbt_get_left(node);
        struct RBTree *right_ch = rbt_get_right(node);
        if (!rbt_isempty(left_ch)) {
                retcode |= subtree_destruct(left_ch);
        }
        if (!rbt_isempty(right_ch)) {
                retcode |= subtree_destruct(right_ch);
        }
        destruct(node);
        return retcode;
}

static enum Side get_side(struct RBTree *node)
{
        assert(node);

        if (!rbt_isroot(node)) {
                struct RBTree *parent = rbt_get_parent(node);
                if (rbt_get_left(parent) == node) {
                        return LEFT;
                } else if(rbt_get_right(parent) == node) {
                        return RIGHT;
                }
        }

        return ROOT;
}

#ifndef NDEBUG

static void dump_cb(value_t val, struct RBTree *t, void *file_ptr)
{
        FILE* file = *(FILE**)file_ptr;
        struct RBTree *left_ch = rbt_get_left(t);
        struct RBTree *right_ch = rbt_get_right(t);
        fprintf(file, "%d [style=\"filled\", ",  val);
        if (get_color(t) == RED) {
                fprintf(file, "fillcolor=\"red\"];\n");
        } else {
                fprintf(file, "fillcolor=\"lightgrey\"];\n");
        }
        
        if (!rbt_isempty(left_ch)) {
                fprintf(file, "%d -> %d [label=\"L\"]\n", val, rbt_get_val(left_ch, NULL));
        }
        if (!rbt_isempty(right_ch)) {
                fprintf(file, "%d -> %d[label=\"R\"]\n", val, rbt_get_val(right_ch, NULL));
        }
}

void rbt_dump(struct RBTree *tree, const char* filename)
{
        FILE* file = fopen(filename, "w");
        fprintf(file, "digraph G {\n");
        rbt_foreach(tree, dump_cb, (void*)&file);
        fprintf(file, "}");
        fclose(file);
}

#endif
