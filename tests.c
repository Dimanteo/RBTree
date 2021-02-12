#include <stdio.h>
#include "RBTree.h"

int main() 
{
        /* Pseudocode examples of interfaces
         * RBTree tree;
         * rbt_init(&tree);
         * rbt_insert(&tree, 10);
         * rbt_find(&tree, 10);
         * rbt_remove(&tree, 10);
         * rbt_foreach(&tree, callback, data);
         * rbt_destruct(&tree); 
         */

        struct RBTree *tree = rbt_init(10);
        rbt_insert(tree, 10);
        rbt_insert(tree, 20);
        rbt_insert(tree, 5);
        rbt_insert(tree, 5);
        rbt_destruct(tree);

        return 0;
}