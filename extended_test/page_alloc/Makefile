
MACROS = ./macros.h
INCLUDES = ../../src/include

CC = gcc -I $(INCLUDES) -imacros $(MACROS)

page_alloc:
	$(CC) ../../src/lib/page_alloc/page_alloc.c

.PHONY: test
test: page_alloc
	$(CC) ./test_page_alloc.c

