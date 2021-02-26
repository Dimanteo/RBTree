CC := gcc
CFLAGS := -Wall -Wextra -MD -g --coverage -O0

test.out: test.o RBTree.o
	$(CC) --coverage tests.o RBTree.o -o test.out

gcov: test.out
	gcov -d -f RBTree.c

RBTree.o : RBTree.c

%.png : %.dot
	dot -Tpng $< -o $@

.PHONY: clean
clean:
	rm -rf *.o *.d test.out *.dot *.png  *.gcov *.gcno *.gcda

-include *.d