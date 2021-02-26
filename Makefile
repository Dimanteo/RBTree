CC := gcc
CFLAGS := -Wall -Wextra -MD -c

test.out: test.o RBTree.o
	$(CC) $^ -o test.out

debug: testd.o RBTreed.o
	$(CC) --coverage $^ -o dbgtest.out

gcov: test.out
	gcov -d -f RBTree.c

%d.o: %.c
	$(CC) $(CFLAGS) -g --coverage -O0 $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -DNDEBUG $< -o $@

%.png : %.dot
	dot -Tpng $< -o $@

.PHONY: clean
clean:
	rm -rf *.o *.d test.out *.dot *.png  *.gcov *.gcno *.gcda

-include *.d