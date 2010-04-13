
#MACROS = ./macros.h
#INCLUDES = ../../src/include
#INCLUDES = ./

#CC = gcc -I $(INCLUDES) -imacros $(MACROS) # -D EXTENDED_TEST
#CC = gcc -include ../../src/include/lib/page_alloc.h -imacros $(MACROS) # -D EXTENDED_TEST
#CC = gcc -include ../../src/include/lib/page_alloc.h -D EXTENDED_TEST
CC = gcc -g -I ./ -D EXTENDED_TEST

.PHONY: test
test: clean page_alloc ./test_page_alloc.c ./config.h ../../src/include/lib/page_alloc.h
	cp -f ../../src/include/lib/page_alloc.h ./lib/
	$(CC) ./test_page_alloc.c page_alloc.o -o test_page_alloc

page_alloc: ../../src/lib/page_alloc/page_alloc.c
	cp -f ../../src/include/lib/page_alloc.h ./lib/
	$(CC) ../../src/lib/page_alloc/page_alloc.c -c -o page_alloc.o

clean:
	rm -f ./lib/page_alloc.h
	rm -f ./page_alloc.o
	rm -f ./test_page_alloc

