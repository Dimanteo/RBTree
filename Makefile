CC := gcc
CFLAGS := -Wall -Wextra -MD -g

test.out: tests.o RBTree.o
	$(CC) tests.o RBTree.o -o test.out

RBTree.o : RBTree.c

clean:
	rm -rf *.o *.d test.out

-include *.d