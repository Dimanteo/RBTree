CC := gcc
CFLAGS := -Wall -Wextra -MD -c
DEBUG_FLAGS := --coverage -I./libfiu/libfiu -DFIU_ENABLE=1 -g -O0
LIB_PATH := ./libfiu/libfiu

release: test.out rbtest.out

all: release debug shared

debug: testd.out rbtestd.out

shared: testsh.out rbtestsh.out

rbtest.out: rbtest.o RBTree.o

test.out: test.o RBTree.o

rbtestd.out: rbtestd.o RBTreed.o

testd.out: testd.o RBTreed.o

%sh.out: %.o RBTree.so
	$(CC) -L. -Wl,-rpath=. -o $@ $< -lRBTree

gcov: debug
	gcov  -d -m RBTreed

docs: RBTree.h doxygen-config
	doxygen doxygen-config

%d.out : %d.o
	$(CC) -L$(LIB_PATH) -Wl,-rpath=$(LIB_PATH) --coverage -o $@ $^ -lfiu

%d.o: %.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $< -o $@

%.out : %.o
	$(CC) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -DNDEBUG $< -o $@

%.so : %.c
	$(CC) -fpic $(CFLAGS) -DNDEBUG -o $(basename $<)pic.o $<
	$(CC) --shared -o lib$(basename $<).so $(basename $<)pic.o

%.png : %.dot
	dot -Tpng $< -o $@

.PHONY: libfiu
libfiu:
	$(MAKE) -C libfiu

libfiu_clean:
	$(MAKE) -C libfiu clean

.PHONY: clean
clean:
	rm -rf *.o *.d *.dot *.png  *.gcov *.gcno *.gcda *.so \
	test.out testd.out rbtest.out rbtestd.out

-include *.d