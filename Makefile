CC := gcc
CFLAGS := -Wall -Wextra -MD -c

release: test.out rbtest.out

debug: testd.out rbtestd.out

rbtest.out: rbtest.o RBTree.o

test.out: test.o RBTree.o

rbtestd.out: rbtestd.o RBTreed.o

testd.out: testd.o RBTreed.o

gcov: debug
	gcov  -d -m RBTreed

docs: RBTree.h doxygen-config
	doxygen doxygen-config

%d.out : %d.o
	$(CC) --coverage  $^ -o $@

%d.o: %.c
	$(CC) --coverage $(CFLAGS) -g -O0 $< -o $@

%.out : %.o
	$(CC) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -DNDEBUG $< -o $@

%.png : %.dot
	dot -Tpng $< -o $@

.PHONY: clean
clean:
	rm -rf *.o *.d *.dot *.png  *.gcov *.gcno *.gcda \
	test.out testd.out rbtest.out rbtestd.out

-include *.d