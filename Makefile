CC := gcc
CFLAGS := -Wall -Wextra -MD -g --coverage
GVIZ_FILE := rbtree

test.out: tests.o RBTree.o
	$(CC) --coverage tests.o RBTree.o -o test.out

gcov: test.out
	./test.out
	gcov RBTree.c

RBTree.o : RBTree.c

dump:
	dot -Tpng $(GVIZ_FILE).dot -o $(GVIZ_FILE).png

valgrind:
	valgrind --leak-check=full -s ./test.out

clean:
	rm -rf *.o *.d test.out $(GVIZ_FILE).dot $(GVIZ_FILE).png  *.gcov \
	*.gcno *.gcda

-include *.d