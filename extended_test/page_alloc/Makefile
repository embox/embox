
#MACROS = ./macros.h
#INCLUDES = ../../src/include
#INCLUDES = ./

#CC = gcc -I $(INCLUDES) -imacros $(MACROS) # -D EXTENDED_TEST
#CC = gcc -include ../../src/include/lib/page_alloc.h -imacros $(MACROS) # -D EXTENDED_TEST
#CC = gcc -include ../../src/include/lib/page_alloc.h -D EXTENDED_TEST
CC = gcc -g -I ./ -D EXTENDED_TEST -imacros config.h

.PHONY: test
test: clean page_alloc ./test_page_alloc.c ./config.h ../../src/include/kernel/mm/opallocator.h.bak
	cp -f ../../src/include/kernel/mm/opallocator.h.bak ./kernel/mm/opallocator.h
	$(CC) ./test_page_alloc.c page_alloc.o -o test_page_alloc

page_alloc: ../../src/kernel/mm/opallocator/opallocator_list.c
	cp -f ../../src/include/kernel/mm/opallocator.h.bak ./kernel/mm/opallocator.h
	$(CC) ../../src/kernel/mm/opallocator/opallocator_list.c -c -o page_alloc.o

clean:
	rm -f ./kernel/mm/*
	rm -f ./page_alloc.o
	rm -f ./test_page_alloc

