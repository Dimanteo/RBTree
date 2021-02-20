CC := gcc
CFLAGS := -Wall -Wextra -MD -g
GVIZ_FILE := rbtree

test.out: tests.o RBTree.o
	$(CC) tests.o RBTree.o -o test.out

RBTree.o : RBTree.c

dump:
	dot -Tpng $(GVIZ_FILE).dot -o $(GVIZ_FILE).png

clean:
	rm -rf *.o *.d test.out $(GVIZ_FILE).dot $(GVIZ_FILE).png

-include *.d