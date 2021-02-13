#include <stdio.h>
#include "RBTree.h"

void printer(value_t val, struct RBTree *tree, void *nodes_n)
{
        *(int*)nodes_n += 1;
        printf("Node[%p]->val = %d\n", tree, val);
}

int main() 
{
        struct RBTree *tree = rbt_init(10);
        rbt_insert(tree, 10);
        rbt_insert(tree, 20);
        rbt_insert(tree, 5);
        rbt_insert(tree, 5);
        int nodes_n = 0;
        rbt_foreach(tree, printer, (void*)&nodes_n);
        printf("Total nodes: %d\n", nodes_n);
        int search_val = 20;
        struct RBTree *node = rbt_find(tree, search_val);
        printf("Found %d in [%p]->val = %d\n", search_val, node, rbt_get_val(node, NULL));
        rbt_destruct(tree);
        return 0;
}