
CC = gcc -g -I ./ -D EXTENDED_TEST -imacros ./config.h

.PHONY: test
test: clean multipage_alloc ./test_multipage_alloc.c ./config.h ../../src/include/lib/multipage_alloc.h
	@cp -f ../../src/include/lib/multipage_alloc.h ./lib/
	$(CC) ./test_multipage_alloc.c multipage_alloc.o -o test_multipage_alloc

multipage_alloc: ../../src/kernel/mm/pallocator/pallocator_almostbuddy.c
	@cp -f ../../src/include/lib/multipage_alloc.h ./lib/
	$(CC) ../../src/kernel/mm/pallocator/pallocator_almostbuddy.c -c -o multipage_alloc.o

clean:
	@echo Remove some file
	@rm -f ./lib/multipage_alloc.h
	@rm -f ./multipage_alloc.o
	@rm -f ./test_multipage_alloc
	@rm -f ./simpltest

simpltest.c: clean multipage_alloc ./test_multipage_alloc.c ./config.h ../../src/include/lib/multipage_alloc.h
	@cp -f ../../src/include/lib/multipage_alloc.h ./lib/
	$(CC) ./simpltest.c multipage_alloc.o -o simpltest

