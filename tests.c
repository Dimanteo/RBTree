#include <stdio.h>
#include "RBTree.h"

void printer(value_t val, struct RBTree *tree, void *nodes_n)
{
        *(int*)nodes_n += 1;
        printf("Node[%p]->val = %d\n", tree, val);
}

int main() 
{
        struct RBTree *tree = rbt_init(13);
        rbt_insert(tree, 8);
        rbt_insert(tree, 17);
        rbt_insert(tree, 1);
        rbt_insert(tree, 6);
        rbt_insert(tree, 11);
        rbt_insert(tree, 17);
        rbt_insert(tree, 15);
        rbt_insert(tree, 25);
        rbt_insert(tree, 22);
        rbt_insert(tree, 27);
        int nodes_n = 0;
        rbt_foreach(tree, printer, (void*)&nodes_n);
        printf("Total nodes: %d\n", nodes_n);
        int search_val = 13;
        struct RBTree *node = rbt_find(tree, search_val);
        printf("Found %d in [%p]->val = %d\n", search_val, node, rbt_get_val(node, NULL));
        #ifndef NDEBUG
                rbt_dump(tree, "rbtree.dot");
        #endif
        rbt_destruct(tree);
        return 0;
}