
create_linked_file:
	gcc -c test_prog.c -o test_prog
	ld -nostdlib -T test_prog.lds -o test_prog_linked test_prog
	rm -f test_prog

create_disassemled_file: create_linked_file
	objdump -S test_prog_linked>test_prog_linked.dis

main: create_linked_file
	gcc elf_writer.c -o elf_writer

execute: main
	./elf_writer test_prog_linked

clean:
	rm -f elf_writer
	rm -f test_prog_linked.dis
	rm -f test_prog_linked
	rm -f test_prog


