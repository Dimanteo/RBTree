CC := gcc
CFLAGS := -Wall -Wextra -MD -g --coverage

test.out: tests.o RBTree.o
	$(CC) --coverage tests.o RBTree.o -o test.out

gcov: test.out
	./test.out
	gcov RBTree.c

RBTree.o : RBTree.c

%.png : %.dot
	dot -Tpng $< -o $@

valgrind:
	valgrind --leak-check=full -s ./test.out

clean:
	rm -rf *.o *.d test.out *.dot *.png  *.gcov *.gcno *.gcda

-include *.d