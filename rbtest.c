#include "RBTree.h"

#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#define DOTFILE(test, n) #test "-" #n ".dot"

static unsigned long Seed = 42;

int check(int cond, int test, int line);

unsigned long getul(char* argv);

void test1(int test)
{
        struct RBTree *tree = rbt_init();
        assert(rbt_get_size(tree) == 0);
        check(rbt_contains(tree, 0) == 0, 1, __LINE__);
        rbt_destruct(tree);
}

void test2(int test)
{
        rbt_destruct(NULL);
        rbt_insert(NULL, 1);
        rbt_remove(NULL, 1);
        rbt_contains(NULL, 1);
        rbt_foreach(NULL, NULL, NULL);
}

void test3(int test)
{
        struct RBTree *tree = rbt_init();
        rbt_insert(tree, 1);
        rbt_insert(tree, 2);
        rbt_insert(tree, 3);
        rbt_insert(tree, 4);
        check(rbt_get_size(tree) == 4, test, __LINE__);
        rbt_dump(tree, DOTFILE(3, 1));
        rbt_remove(tree, 2);
        check(rbt_get_size(tree) == 3, test, __LINE__);
        rbt_dump(tree, DOTFILE(3, 2));
        rbt_destruct(tree);
}

void test4(int test)
{
        struct RBTree *tree = rbt_init();
        rbt_insert(tree, 4);
        rbt_insert(tree, 3);
        rbt_insert(tree, 2);
        rbt_insert(tree, 1);
        check(rbt_get_size(tree) == 4, test, __LINE__);
        rbt_dump(tree, DOTFILE(4, 1));
        rbt_remove(tree, 3);
        check(rbt_get_size(tree) == 3, test, __LINE__);
        rbt_dump(tree, DOTFILE(4, 2));
        rbt_destruct(tree);

}

void test5(int test)
{
        struct RBTree *tree = rbt_init();
        check(tree != NULL, test, __LINE__);
        int n = 15;
        for (int i = 0; i < n; i++) {
                check(rbt_insert(tree, i) == 1, test, __LINE__);
                check(rbt_contains(tree, i), test, __LINE__);
        }
        check(rbt_get_size(tree) == (size_t)n, test, __LINE__);
        rbt_dump(tree, DOTFILE(5, 1));
        for (int i = 0; i < n; i++) {
                check(rbt_remove(tree, i), test, __LINE__);
                check(rbt_contains(tree, i) == 0, test, __LINE__);
        }
        rbt_dump(tree, DOTFILE(5, 2));
        check(rbt_get_size(tree) == 0, test, __LINE__);
        check(rbt_destruct(tree) == 0, test, __LINE__);
}

void test6(int test)
{
        struct RBTree *tree = rbt_init();
        check(rbt_contains(tree, 42) == 0, test, __LINE__);
        rbt_insert(tree, 42);
        check(rbt_contains(tree, 42), test, __LINE__);
        rbt_destruct(tree);
}

int t7_comparator(const void* lptr, const void* rptr)
{
        int lhs = *(int*)lptr;
        int rhs = *(int*)rptr;
        return lhs - rhs;
}

void t7_callback(value_t val, struct RBTree* t, void* out)
{
        int index = ((int*)out)[0];
        ((int*)out)[index++] = val;
        ((int*)out)[0] = index;
}

void test7(int test)
{
        srand(Seed);
        struct RBTree *tree = rbt_init();
        static const size_t N = 20;
        int entry[N];
        int output[N + 1];
        output[0] = 1;

        for (size_t i = 0; i < N; i++)
        {
                entry[i] = rand() % N;
                while (rbt_contains(tree, entry[i])) {
                        entry[i] = rand() % 1000;
                }
                rbt_insert(tree, entry[i]);
        }

        qsort((void*)entry, N, sizeof(entry[0]), t7_comparator);

        rbt_foreach(tree, t7_callback, (void*)output);

        rbt_dump(tree, DOTFILE(7, 1));

        for (size_t i = 0; i < N; i++)
        {
                check(entry[i] == output[i + 1], test, __LINE__);
        }

        rbt_destruct(tree);
}

void test8(int test)
{
        struct RBTree *tree = rbt_init();
        for (size_t i = 0; i < 15; i++)
        {
                rbt_insert(tree, (i + 6) % 16);
        }
        rbt_dump(tree, DOTFILE(8, 1));
        rbt_remove(tree, 9);
        rbt_dump(tree, DOTFILE(8, 2));
        rbt_destruct(tree);
}

void test9(int test)
{
        struct RBTree *tree = rbt_init();
        size_t N = 8;
        for (size_t i = 0; i < N; i++)
        {
                check(rbt_insert(tree, i) == 1, test, __LINE__);
                check(rbt_insert(tree, i) == 0, test, __LINE__);
        }
        check(rbt_get_size(tree) == N, test, __LINE__);
        rbt_dump(tree, DOTFILE(9, 1));
        for (size_t i = 0; i < N; i++)
        {
                check(rbt_remove(tree, i) == 1, test, __LINE__);
                check(rbt_remove(tree, i) == 0, test, __LINE__);
        }
        check(rbt_get_size(tree) == 0, test, __LINE__);
        rbt_dump(tree, DOTFILE(9, 2));
        for (size_t i = 0; i < N; i++)
        {
                check(rbt_insert(tree, i) == 1, test, __LINE__);
        }
        rbt_dump(tree, DOTFILE(9, 3));
        rbt_destruct(tree);
}

void t10_callback(value_t val, struct RBTree* tree, void* cnt)
{
        (*(size_t*)cnt)++;
        rbt_remove(tree, val + 1);
}

void test10(int test)
{
        struct RBTree *tree = rbt_init();
        for (size_t i = 1; i <= 10; i++) {
                check(rbt_insert(tree, i) == 1, test, __LINE__);
        }
        size_t size = rbt_get_size(tree);
        size_t counter = 0;
        rbt_foreach(tree, t10_callback, (void*)&counter);
        check(counter == size, test, __LINE__);
        check(rbt_get_size(tree) == 1, test, __LINE__);
        rbt_destruct(tree);
}

void t11_callback(value_t val, struct RBTree* tree, void* cnt)
{
        (*(size_t*)cnt)++;
        rbt_remove(tree, val - 1);
}

void test11(int test)
{
        struct RBTree *tree = rbt_init();
        for (size_t i = 1; i <= 10; i++) {
                check(rbt_insert(tree, i) == 1, test, __LINE__);
        }
        size_t size = rbt_get_size(tree);
        size_t counter = 0;
        rbt_foreach(tree, t10_callback, (void*)&counter);
        check(counter == size, test, __LINE__);
        check(rbt_get_size(tree) == 1, test, __LINE__);
        rbt_destruct(tree);
}

void t12_callback(value_t val, struct RBTree* tree, void* cnt)
{
        (*(size_t*)cnt)++;
        rbt_remove(tree, val);
}

void test12(int test)
{
        struct RBTree *tree = rbt_init();
        for (size_t i = 1; i <= 10; i++) {
                check(rbt_insert(tree, i) == 1, test, __LINE__);
        }
        size_t size = rbt_get_size(tree);
        size_t counter = 0;
        rbt_foreach(tree, t10_callback, (void*)&counter);
        check(counter == size, test, __LINE__);
        check(rbt_get_size(tree) == 1, test, __LINE__);
        rbt_destruct(tree);
}

void test13(int test)
{
        struct RBTree *tree = rbt_init();
        check(rbt_insert(tree, 2), test, __LINE__);
        check(rbt_insert(tree, 4), test, __LINE__);
        check(rbt_insert(tree, 3), test, __LINE__);
        check(rbt_get_size(tree) == 3, test, __LINE__);
        rbt_destruct(tree);
}

void test14(int test)
{
        struct RBTree *tree = rbt_init();
        size_t N = 15;
        for (size_t i = N; i > 0; i--) {
                check(rbt_insert(tree, i), test, __LINE__);
        }
        check(rbt_get_size(tree) == N, test, __LINE__);
        for (size_t i = N; i > 0; i--) {
                check(rbt_remove(tree, i), test, __LINE__);
        }
        rbt_destruct(tree);
}

void test15(int test)
{
#ifndef NDEBUG
        malloc_fail_enable();
        struct RBTree *tree = rbt_init();
        check(tree == NULL, test, __LINE__);
        malloc_fail_disable();

        tree = rbt_init();

        malloc_fail_enable();
        check(rbt_insert(tree, 42) == -1, test, __LINE__);
        malloc_fail_disable();

        check(rbt_insert(tree, 3), test, __LINE__);
        check(rbt_insert(tree, 2), test, __LINE__);
        check(rbt_insert(tree, 5), test, __LINE__);

        malloc_fail_enable();
        check(rbt_insert(tree, 52), test, __LINE__);
        malloc_fail_disable();

        check(rbt_contains(tree, 52) == 0, test, __LINE__);
        check(rbt_insert(tree, 1) == 1, test, __LINE__);
        rbt_dump(tree, DOTFILE(15, 1));
        rbt_destruct(tree);
#endif
}

void test16(int test)
{
        size_t N = 1000;
        int *entry = alloca(sizeof(int) * N);
        srand(Seed);
        struct RBTree *tree = rbt_init();
        check(tree != NULL, test, __LINE__);
        for (size_t i = 0; i < N; i++) {
                entry[i] = rand() % N;
                int ret = rbt_insert(tree, entry[i]);
                check(ret != -1, test, __LINE__);
                if (ret == 0) {
                        entry[i] = -1;
                }
        }
        for (size_t i = 0; i < N; i++) {
                int ret = rbt_remove(tree, entry[i]);
                check(ret != -1, test, __LINE__);
                if (entry[i] != -1) {
                        check(ret == 1, test, __LINE__);
                } else {
                        check(ret == 0, test, __LINE__);
                }
        }
        check(rbt_destruct(tree) == 0, test, __LINE__);
}

int main(int argc, char **argv)
{
        if (argc > 1) {
                Seed = getul(argv[1]);
        }
        test1(1);
        test2(2);
        test3(3);
        test4(4);
        test5(5);
        test6(6);
        test7(7);
        test8(8);
        test9(9);
        test10(10);
        test11(11);
        test12(12);
        test13(13);
        test14(14);
        test15(15);
        test16(16);
        return 0;
}

int check(int cond, int test, int line)
{
        if (!cond) {
                printf("%d: Test %d failed\n", line, test);
                return 0;
        }
        return 1;
}

unsigned long getul(char* argv)
{
    char* endptr = NULL;
    unsigned long ret_val = strtoul(argv, &endptr, 10);

    if (*endptr != '\0')
    {
        fprintf(stderr, "Conversion error %s. Invalid symbol: %c\n", argv, *endptr);
        exit(EXIT_FAILURE);
    }
    if (ret_val == ULONG_MAX && errno == ERANGE)
    {
        fprintf(stderr, "Overflow occured\n");
        exit(EXIT_FAILURE);
    }
    return ret_val;
}
