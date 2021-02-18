CC := gcc
CFLAGS := -Wall -Wextra -MD -g

test.out: tests.o RBTree.o
	$(CC) tests.o RBTree.o -o test.out

RBTree.o : RBTree.c

dump:
	dot -Tpng rbtree.dot -o rbtree.png

clean:
	rm -rf *.o *.d test.out rbtree.dot rbtree.png

-include *.d