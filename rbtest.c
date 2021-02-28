#include "RBTree.h"

int check(int cond, const char *errmsg, int line);

int main()
{
        // test 1
        struct RBTree *tree = rbt_init();
        assert(rbt_get_size(tree) == 0);
        rbt_destruct(tree);

        // test 2
        rbt_destruct(NULL);
        rbt_insert(NULL, 1);
        rbt_remove(NULL, 1);
        rbt_contains(NULL, 1);
        rbt_foreach(NULL, NULL, NULL);
        
        // test 3 covers rm case 6
        tree = rbt_init();

        rbt_insert(tree, 1);
        rbt_insert(tree, 2);
        rbt_insert(tree, 3);
        rbt_insert(tree, 4);
        check(rbt_get_size(tree) == 4, "incorrect size", __LINE__);
        rbt_dump(tree, "3-1.dot");
        rbt_remove(tree, 2);
        check(rbt_get_size(tree) == 3, "incorrect size", __LINE__);
        rbt_dump(tree, "3-2.dot");

        rbt_destruct(tree);

        // test 4
        tree = rbt_init();

        rbt_insert(tree, 4);
        rbt_insert(tree, 3);
        rbt_insert(tree, 2);
        rbt_insert(tree, 1);
        check(rbt_get_size(tree) == 4, "incorrect size", __LINE__);
        rbt_dump(tree, "4-1.dot");
        rbt_remove(tree, 3);
        check(rbt_get_size(tree) == 3, "incorrect size", __LINE__);
        rbt_dump(tree, "4-2.dot");

        rbt_destruct(tree);

        return 0;
}

int check(int cond, const char *errmsg, int line)
{
        if (!cond) {
                printf("%d: Test failed: %s\n", line, errmsg);
                return 0;
        }
        return 1;
}