
#MACROS = ./macros.h
#INCLUDES = ../../src/include
#INCLUDES = ./

#CC = gcc -I $(INCLUDES) -imacros $(MACROS) # -D EXTENDED_TEST
#CC = gcc -include ../../src/include/lib/page_alloc.h -imacros $(MACROS) # -D EXTENDED_TEST
#CC = gcc -include ../../src/include/lib/page_alloc.h -D EXTENDED_TEST
CC = gcc -g -I ./ -D EXTENDED_TEST

.PHONY: test
test: page_alloc
	cp ../../src/include/lib/page_alloc.h ./lib/
	$(CC) ./test_page_alloc.c page_alloc -o test_page_alloc

page_alloc:
	cp ../../src/include/lib/page_alloc.h ./lib/
	$(CC) ../../src/lib/page_alloc/page_alloc.c -c -o page_alloc

clean:
	rm ./lib/page_alloc.h
	rm ./page_alloc
	rm ./test_page_alloc

