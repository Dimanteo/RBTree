#include <stdio.h>
#include "RBTree.h"

void printer(value_t val, struct RBTree *tree, void *nodes_n)
{
        *(int*)nodes_n += 1;
        printf("Node[%p]->val = %d\n", tree, val);
}

int main() 
{
        int nodes_n = 0;
        int search_val = 11;
        int rm_val = 8;
        struct RBTree *tree = rbt_init(13);
        rbt_insert(tree, search_val);
        rbt_insert(tree, rm_val);
        rbt_insert(tree, 17);
        rbt_insert(tree, 1);
        rbt_insert(tree, 6);
        rbt_insert(tree, 11);
        rbt_insert(tree, 17);
        rbt_insert(tree, 15);
        rbt_insert(tree, 25);
        rbt_insert(tree, 22);
        rbt_insert(tree, 27);
        rbt_rmval(tree, 8);
        rbt_foreach(tree, printer, (void*)&nodes_n);
        printf("Total nodes: %d\n", nodes_n);
        struct RBTree *node = rbt_find(tree, search_val);
        printf("Found %d in [%p]->val = %d\n", search_val, node, rbt_get_val(node, NULL));
        node = rbt_find(tree, rm_val);
        if (node != NULL) {
                printf("ERROR: found removed value\n");
        } else {
                printf("Value removed\n");
        }
        #ifndef NDEBUG
                rbt_dump(tree, "rbtree.dot");
        #endif
        rbt_destruct(tree);
        return 0;
}