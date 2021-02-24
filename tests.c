#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "RBTree.h"

/* Query language:
 * ./test.out {[r, f, d] number}
 * r - remove number
 * f - find number
 * d - dump to file <number>.dot */

enum ArgType {ERR, NUM, RM, FIND, DUMP};

static void printer(value_t val, struct RBTree *tree, void *nodes_n);

static enum ArgType get_arg(const char *argi, int *num);

int main(int argc, char **argv)
{
        struct RBTree *tree = rbt_init();
        for (int i = 1; i < argc; i++) {
                int num = 0;
                int retcode = 0;
                enum ArgType argt = get_arg(argv[i], &num);
                if (argt == NUM) {
                        retcode = rbt_insert(tree, num);
                } else {
                        enum ArgType numarg = get_arg(argv[i + 1], &num);
                        if (numarg != NUM) {
                                argt = ERR;
                        }
                        switch (argt)
                        {
                        case RM:
                                retcode = rbt_rmval(tree, num);
                                break;
                        case FIND:
                                retcode = rbt_find(tree, num);
                                break;
                        case DUMP: {
                                #ifndef NDEBUG
                                        char fname[sizeof(num) * 2 + 5];
                                        sprintf(fname, "%x.dot", num);
                                        rbt_dump(tree, fname);
                                #endif
                                break;
                        }
                        default:
                                fprintf(stderr, "Argument ignored: %s\n", argv[i]);
                                i--;
                                break;
                        }
                        i++;
                }
                printf("arg[%d] : code[%d]\n", i, retcode);
        }
        rbt_destruct(tree);
        return 0;
}

static void printer(value_t val, struct RBTree *tree, void *nodes_n)
{
        *(int*)nodes_n += 1;
        printf("Node[%p]->val = %d\n", tree, val);
}

static enum ArgType get_arg(const char *argi, int *num)
{
        switch (*argi)
        {
        case 'r':
                return RM;
                break;
        case 'f':
                return FIND;
                break;
        case 'd':
                return DUMP;
                break;
        }
        char* endptr = NULL;
        *num = strtol(argi, &endptr, 10);

        if (*endptr != '\0')
        {
                fprintf(stderr, "Conversion error %s. Invalid symbol: %c\n", argi, *endptr);
                return ERR;
        }
        if (*num == LONG_MAX && errno == ERANGE)
        {
                fprintf(stderr, "Overflow occured\n");
                return ERR;
        }
        if (*num == LONG_MIN && errno == ERANGE)
        {
                fprintf(stderr, "Underflow occured\n");
                return ERR;
        }
        return NUM;
}
