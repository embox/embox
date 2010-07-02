
FILE_FOR_TEST = mpallocator_buddy.c

CC = gcc -g -I ./ -D EXTENDED_TEST -imacros ./config.h

.PHONY: test
test: clean multipage_alloc ./test_multipage_alloc.c ./config.h ../../src/include/kernel/mm/mpallocator.h.bak
	@cp -f ../../src/include/kernel/mm/mpallocator.h.bak ./kernel/mm/mpallocator.h
	$(CC) ./test_multipage_alloc.c multipage_alloc.o -o test_multipage_alloc

multipage_alloc: ../../src/kernel/mm/mpallocator/mpallocator_buddy.c
	@cp -f ../../src/include/kernel/mm/mpallocator.h.bak ./kernel/mm/mpallocator.h
	$(CC) ../../src/kernel/mm/mpallocator/$(FILE_FOR_TEST) -c -o multipage_alloc.o

clean:
	@echo Remove some file
	@rm -f ./kernel/mm/*
	@rm -f ./multipage_alloc.o
	@rm -f ./test_multipage_alloc
	@rm -f ./simpltest

simpltest.c: clean multipage_alloc ./test_multipage_alloc.c ./config.h
	@cp -f ../../src/include/kernel/mm/mpallocator.h.bak ./kernel/mm/mpallocator.h
	$(CC) ./simpltest.c multipage_alloc.o -o simpltest

