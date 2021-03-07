#include "RBTree.h"
#include <fiu-local.h>

enum Color {BLACK, RED};
enum Side {LEFT = 0, RIGHT = 1, ROOT = -1, PSEUDO = -2, NONE = -3};

struct RBTree {
        value_t value;
        enum Color color;
        struct RBTree *children[2];
        struct RBTree *parent;
        size_t node_count;
};

static struct RBTree *create_node();

static void destruct(struct RBTree *node);

static void subtree_destruct(struct RBTree *node);

static int insert(struct RBTree *tree, value_t val);

static struct RBTree *find(struct RBTree *node, value_t val);

static value_t *foreach(struct RBTree *node, value_t *values);

static int isempty(const struct RBTree *child);

static int isroot(const struct RBTree *node);

static int ispseudo(const struct RBTree *node);

static struct RBTree *get_left(const struct RBTree *tree);

static struct RBTree *get_right(const struct RBTree *tree);

static struct RBTree *get_parent(const struct RBTree *tree);

static struct RBTree *get_sibling(const struct RBTree *node);

static value_t get_val(const struct RBTree *tree);

static void set_val(struct RBTree *node, value_t val);

static enum Color get_color(const struct RBTree *tree);

static void set_color(struct RBTree *node, enum Color clr);

static enum Side get_side(const struct RBTree *node);

static void insert_balance(struct RBTree *node);

static void remove_balance(struct RBTree *node);

static void rotate_left(struct RBTree *node);

static void rotate_right(struct RBTree *node);

static void set_child(struct RBTree *parent, struct RBTree *child, enum Side side);

static struct RBTree *get_rightmost(const struct RBTree *node);

static int verify_balance(struct RBTree *node);

static void* fiu_malloc(size_t size);

struct RBTree *rbt_init()
{
        struct RBTree *tree = create_node();
        if (tree == NULL) {
                return NULL;
        }
        tree->node_count = 0;
        tree->parent = NULL;
        set_child(tree, NULL, ROOT);
        assert(ispseudo(tree));
        return tree;
}

int rbt_destruct(struct RBTree *tree) 
{
        if (tree == NULL) {
                return -1;
        }
        struct RBTree *node = get_left(tree);
        if (!isempty(node)) {
                subtree_destruct(node);
        }

        destruct(tree);

        return 0;
}

int rbt_insert(struct RBTree *tree, value_t val)
{
        if (tree == NULL) {
                return -1;
        }
        struct RBTree *node = get_left(tree);
        int retcode = 0;
        if (isempty(node)) {
                node = create_node();
                if (node == NULL) {
                        return -1;
                }
                set_color(node, BLACK);
                set_val(node, val);
                set_child(node, NULL, ROOT);
                set_child(tree, node, ROOT);
                retcode = 1;
        } else {
                retcode = insert(node, val);
        }

        if (retcode == 1) {
                tree->node_count++;
        }
        assert(ispseudo(tree));
        verify_balance(get_left(tree));
        return retcode;
}

int rbt_contains(const struct RBTree *tree, value_t val)
{
        if (tree == NULL) {
                return 0;
        }

        struct RBTree *node = get_left(tree);
        if (isempty(node)) {
                return 0;
        }
        node = find(node, val);
        if (node == NULL) {
                return 0;
        }
        assert(get_val(node) == val);
        return 1;
}

int rbt_foreach(struct RBTree *tree, 
                void(*callback)(value_t, struct RBTree*, void*), void *data)
{
        if (!tree || !callback) {
                return -1;
        }

        struct RBTree *node = get_left(tree);
        if (isempty(node)) {
                return 0;
        }

        size_t tree_size = rbt_get_size(tree);
        // According to man, if alloca fails, program behavior is undefined \_('_')_/
        value_t *values = alloca(tree_size * sizeof(values[0]));
        foreach(node, values);
        for (size_t i = 0; i < tree_size; i++) {
                callback(values[i], tree, data);
        }
        
        return 0;
}

int rbt_remove(struct RBTree *tree, value_t val) 
{
        if (tree == NULL) {
                return -1;
        }
        if (isempty(get_left(tree))) {
                return 0;
        }
        struct RBTree *node = find(get_left(tree), val);
        if (node == NULL) {
                return 0;
        }

        /* Reducing to case of deleting node with at least one
         * empty child. Because maximum in left subtree
         * can't have right child. */
        if (!isempty(get_right(node)) && 
                        !isempty(get_left(node))) {
                struct RBTree *rmost = get_rightmost(get_left(node));
                set_val(node, get_val(rmost));
                node = rmost;
        }

        if (get_color(node) == BLACK) {
                struct RBTree *child = get_left(node);
                if (isempty(child)) {
                        child = get_right(node);
                }

                if (get_color(child) == RED) {
                        set_color(child, BLACK);
                        enum Side sd = get_side(node);
                        struct RBTree *parent = get_parent(node);
                        set_child(parent, child, sd);
                } else {
                        // This can happen only if both children are empty
                        remove_balance(node);
                }
        }
        destruct(node);
        tree->node_count--;
        assert(ispseudo(tree));
        verify_balance(get_left(tree));
        return 1;
}

size_t rbt_get_size(struct RBTree *tree)
{
        return tree->node_count;
}

static int isempty(const struct RBTree *leaf)
{
        if (leaf == NULL) {
                return 1;
        }
        return 0;
}

static int isroot(const struct RBTree *node)
{
        assert(node);
        struct RBTree *parent = get_parent(node);
        if (ispseudo(parent)) {
                return 1;
        } else {
                return 0;
        }
}

static int ispseudo(const struct RBTree *node)
{
        if (node == NULL) {
                return 0;
        }
        if (get_parent(node) == node) {
                return 1;
        }
        return 0;
}

static struct RBTree *create_node()
{
        struct RBTree *node = fiu_malloc(sizeof(*node));
        if (node == NULL) {
                return NULL;
        }
        node->value = 0;
        node->color = BLACK;
        node->children[LEFT] = NULL;
        node->children[RIGHT] = NULL;
        node->parent = NULL;
        node->node_count = -1;
        return node;
}

static int insert(struct RBTree *node, value_t val)
{
        assert(node);
        assert(!ispseudo(node));

        enum Side child_side;
        struct RBTree *child;

        if (val > get_val(node)) {
                child_side = RIGHT;
                child = get_right(node);
        } else if (val < get_val(node)) {
                child_side = LEFT;
                child = get_left(node);
        } else {
                return 0;
        }
        int retcode = 0;
        if (isempty(child)) {
                struct RBTree *tmp = create_node();
                if (tmp == NULL) {
                        return -1;
                }
                set_color(tmp, RED);
                set_val(tmp, val);
                set_child(node, tmp, child_side);
                insert_balance(tmp);
                retcode = 1;
        } else {
                retcode = insert(child, val);
        }

        return retcode;;
}

static struct RBTree *find(struct RBTree *node, value_t val)
{
        assert(node);
        assert(!ispseudo(node));

        value_t cur_val = get_val(node);
        if (val == cur_val) {
                return node;
        }
        struct RBTree *ret_node = NULL;
        if (val < cur_val) {
                struct RBTree *left_ch = get_left(node);
                if (!isempty(left_ch)) {
                        ret_node = find(left_ch, val);
                }
        } else {
                struct RBTree *right_ch = get_right(node);
                if (!isempty(right_ch)) {
                        ret_node = find(right_ch, val);
                }
        }

        return ret_node;
}

static value_t *foreach(struct RBTree *node, value_t *values)
{
        assert(node);
        assert(values);
        assert(!ispseudo(node));

        struct RBTree *left_ch = get_left(node);
        struct RBTree *right_ch = get_right(node);
        if (!isempty(left_ch)) {
                values = foreach(left_ch, values);
        }
        values[0] = get_val(node);
        values++;
        if (!isempty(right_ch)) {
                values = foreach(right_ch, values);
        }
        return values;
}

static void insert_balance(struct RBTree *node)
{
        assert(node);

        struct RBTree *parent = NULL;
        struct RBTree *uncle = NULL;
        struct RBTree *granddad = NULL;

        // case 1
        if (isroot(node)) {
                node->color = BLACK;
                return;
        }

        parent = get_parent(node);

        // case 2
        if (get_color(parent) == BLACK) {
                return;
        }

        granddad = get_parent(parent);

        /* looking for uncle
         * and memorizing side of parent for case 4.
         * par_side can't be ROOT because granddad exists */
        enum Side parent_sd = get_side(parent);
        if (parent_sd == LEFT) {
                uncle = get_right(granddad);
        } else { // par_side == RIGHT
                uncle = get_left(granddad);
        }

        /* case 3
         * at this point and further parent.color is RED,
         * because otherwise it would be case 2 */
        if (get_color(uncle) == RED) {
                set_color(parent, BLACK);
                set_color(uncle, BLACK);
                set_color(granddad, RED);
                insert_balance(granddad);
                return;
        }

        /* case 4 - preparation for case 5
         * if uncle.color == BLACK */
        enum Side node_sd = get_side(node);
        if (parent_sd == LEFT && node_sd == RIGHT) {
                        rotate_left(parent);
                        node = get_left(node);
        } else if (parent_sd == RIGHT && node_sd == LEFT) {
                        rotate_right(parent);
                        node = get_right(node);
        }

        // case 5
        parent = get_parent(node);
        granddad = get_parent(parent);
        parent_sd = get_side(parent);
        node_sd = get_side(node);
        set_color(parent, BLACK);
        set_color(granddad, RED);
        if (parent_sd == LEFT && node_sd == LEFT) {
                rotate_right(granddad);
        } else { // parent_sd == RIGHT && node_sd == RIGHT
                rotate_left(granddad);
        }

        return;
}

static void remove_balance(struct RBTree *node)
{
        assert(node);
        /* node color must be BLACK,
         * because RED case was handled in caller function
         * and doesn't require to rebalance tree */
        assert(get_color(node) == BLACK);

        // case 1
        if (isroot(node)) {
                return;
        }

        struct RBTree *parent = get_parent(node);
        struct RBTree *sibling = get_sibling(node);
        struct RBTree *sib_l = get_left(sibling);
        struct RBTree *sib_r = get_right(sibling);

        // case 2
        if (get_color(sibling) == RED) {
                set_color(sibling, BLACK);
                set_color(parent, RED);
                if (get_side(sibling) == RIGHT) {
                        rotate_left(parent);
                        sibling = sib_l;
                } else {
                        rotate_right(parent);
                        sibling = sib_r;
                }
                sib_l = get_left(sibling);
                sib_r = get_right(sibling);
        }
        
        if (get_color(parent) == BLACK && get_color(sibling) == BLACK &&
                get_color(sib_r) == BLACK && get_color(sib_l) == BLACK) {
                // case 3
                // node, parent, sibling and sibling's children are BLACK
                set_color(sibling, RED);
                remove_balance(parent);
                return;
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
        } else if (get_side(node) == LEFT && get_color(sib_l) == RED && get_color(sib_r) == BLACK) {
                // case 5 left is red
                set_color(sib_l, BLACK);
                set_color(sibling, RED);
                rotate_right(sibling);
        } else if ( get_side(node) == RIGHT && get_color(sib_r) == RED && get_color(sib_l) == BLACK) {
                // case 5 right is red
                set_color(sib_r, BLACK);
                set_color(sibling, RED);
                rotate_left(sibling);
        }

        //case 6
        sibling = get_sibling(node);
        sib_l = get_left(sibling);
        sib_r = get_right(sibling);
        set_color(sibling, get_color(parent));
        set_color(parent, BLACK);
        if (get_side(node) == LEFT) {
                if (!isempty(sib_r)) {
                        set_color(sib_r, BLACK);
                }
                rotate_left(parent);
        } else {
                if (!isempty(sib_l)) {
                        set_color(sib_l, BLACK);
                }
                rotate_right(parent);
        }
}

static struct RBTree *get_left(const struct RBTree *tree)
{
        assert(tree);
        return tree->children[LEFT];
}

static struct RBTree *get_right(const struct RBTree *tree) 
{
        assert(tree);
        return tree->children[RIGHT];
}

static struct RBTree *get_parent(const struct RBTree *tree)
{
        assert(tree);

        if (tree->parent == NULL) {
                return tree;
        }

        return tree->parent;
}

static struct RBTree *get_sibling(const struct RBTree *node)
{
        struct RBTree *parent = get_parent(node);
        struct RBTree *sibling = NULL;
        if (get_side(node) == LEFT) {
                sibling = get_right(parent);
        } else {
                sibling = get_left(parent);
        }
        return sibling;
}

static value_t get_val(const struct RBTree *tree) 
{
        assert(tree);
        return tree->value;
}

static enum Color get_color(const struct RBTree *tree)
{
        if (isempty(tree) || tree == NULL) {
                return BLACK;
        }

        return tree->color;
}

static void set_color(struct RBTree *node, enum Color clr)
{
        assert(node);
        node->color = clr;
}

static void set_val(struct RBTree *node, value_t val)
{
        assert(node);
        node->value = val;
}

static void rotate_left(struct RBTree *node)
{
        struct RBTree *pivot = get_right(node);
        assert(pivot);

        struct RBTree *parent = get_parent(node);
        assert(parent);
        if (!isroot(node)) {
                enum Side sd = get_side(node);
                set_child(parent, pivot, sd);
        } else {
                assert(ispseudo(parent));
                set_child(parent, pivot, LEFT);
                set_child(parent, pivot, RIGHT);
        }

        struct RBTree *pivot_l = get_left(pivot);
        set_child(pivot, node, LEFT);
        set_child(node, pivot_l, RIGHT);
}

static void rotate_right(struct RBTree *node)
{
        struct RBTree *pivot = get_left(node);
        assert(pivot);
        
        struct RBTree *parent = get_parent(node);
        assert(parent);
        if (!isroot(node)) {
                enum Side sd = get_side(node);
                set_child(parent, pivot, sd);
        } else {
                assert(ispseudo(parent));
                set_child(parent, pivot, LEFT);
                set_child(parent, pivot, RIGHT);
        }

        struct RBTree *pivot_r = get_right(pivot);
        set_child(pivot, node, RIGHT);
        set_child(node, pivot_r, LEFT);
}

static void set_child(struct RBTree *parent, struct RBTree *child, enum Side side)
{
        assert(side != PSEUDO);
        assert(side != NONE);
        if (ispseudo(parent) || side == ROOT) {
                parent->children[LEFT] = child;
                parent->children[RIGHT] = child;
        } else {
                parent->children[side] = child;        
        }
        if (child != NULL) {
                child->parent = parent;
        }
}

static struct RBTree *get_rightmost(const struct RBTree *node)
{
        struct RBTree *right_ch = get_right(node);
        while (!isempty(right_ch)) {
                node = right_ch;
                right_ch = get_right(node);
        }
        return node;
}

static void destruct(struct RBTree *node)
{
        assert(node);
        enum Side side = get_side(node);
        if (side != NONE && side != PSEUDO) {
                struct RBTree *parent = get_parent(node);
                set_child(parent, NULL, side);
        }
        free(node);
}

static void subtree_destruct(struct RBTree *node)
{
        assert(node);
        assert(!ispseudo(node));

        struct RBTree *left_ch = get_left(node);
        struct RBTree *right_ch = get_right(node);
        if (!isempty(left_ch)) {
                subtree_destruct(left_ch);
        }
        if (!isempty(right_ch)) {
                subtree_destruct(right_ch);
        }
        destruct(node);
}

static enum Side get_side(const struct RBTree *node)
{
        assert(node);

        if (ispseudo(node)) {
                return PSEUDO;
        }

        struct RBTree *parent = get_parent(node);
        struct RBTree *par_l = get_left(parent);
        struct RBTree *par_r = get_right(parent);

        if (isroot(node)) {
                if (par_l == node) {
                        assert(par_l == par_r);
                        return ROOT;
                }
        } else {
                if (get_left(parent) == node) {
                        return LEFT;
                } else if(get_right(parent) == node) {
                        return RIGHT;
                }
        }

        return NONE;
}

#ifndef NDEBUG

static void dump_cb(value_t val, struct RBTree *tree, void *file_ptr)
{
        FILE* file = *(FILE**)file_ptr;
        struct RBTree *node = find(get_left(tree), val);
        assert(node);

        struct RBTree *left_ch = get_left(node);
        struct RBTree *right_ch = get_right(node);
        fprintf(file, "%d [style=\"filled\", ",  val);
        if (get_color(node) == RED) {
                fprintf(file, "fillcolor=\"red\"];\n");
        } else {
                fprintf(file, "fillcolor=\"lightgrey\"];\n");
        }
        
        if (!isempty(left_ch)) {
                fprintf(file, "%d -> %d [label=\"L\"]\n", val, get_val(left_ch));
        }
        if (!isempty(right_ch)) {
                fprintf(file, "%d -> %d[label=\"R\"]\n", val, get_val(right_ch));
        }
}

void rbt_dump(struct RBTree *tree, const char* filename)
{
        assert(tree);
        assert(filename);
        FILE* file = fopen(filename, "w");
        fprintf(file, "digraph G {\n");
        rbt_foreach(tree, dump_cb, (void*)&file);
        fprintf(file, "}");
        fclose(file);
}

static int verify_balance(struct RBTree *node)
{
        if (isempty(node)) {
                return 0;
        }
        int l_deep = verify_balance(get_left(node));
        int r_deep = verify_balance(get_right(node));
        assert(l_deep == r_deep);
        if (get_color(node) == BLACK) {
                return l_deep + 1;
        } else {
                return l_deep;
        }
}

static void* fiu_malloc(size_t size)
{
        fiu_return_on("malloc_failure", NULL);
        return malloc(size);
}

#else

void rbt_dump(struct RBTree *tree, const char* filename) {}

static int verify_balance(struct RBTree *node) {}

#endif
