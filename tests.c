#include <stdio.h>
#include "RBTree.h"

void printer(value_t val, struct RBTree *tree, void *nodes_n)
{
        *(int*)nodes_n++;
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
        rbt_destruct(tree);

        return 0;
}