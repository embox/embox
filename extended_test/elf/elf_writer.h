
/**
 * @date May 1, 2010
 * @author Aleksandr Kirov
 */

#include "elf_reader.h"

		/******LIBRARY INTERFACE******/

/**
 * Function prints consistens of elf header according the specification
 * with comments and meaning writing
 *
 * @param header - head structure which describe common file information
 */
void elf_print_header(Elf32_Ehdr * header);

/**
 * Finds index in array string-table and starts printing name of
 * sm-th begining from that index till the nearest "/0"
 * string_table must start and finish with "/0"
 *
 * @param index - index in array which to start from
 * @param string_table  - array which consist names devided by "/0"
 */
void elf_print_name(int index, char * string_table);

/**
 * Prints given section header using information about reversing of byte-order
 * (big-endianess) with comments and some description of meanings
 *
 * @param section_header - struct to be print
 * @param reversed - info about big-low-endianess data type
 *                 - according specification
 * @param string_table - an array given by user, got by according sectiong
 *                       which func can get names of sections from
 */
void elf_print_section_header(Elf32_Shdr * section_header,
							  uchar reversed,
							  char * string_table);

/**
 * Prints given segment header using information about reversing of byte-order
 * (big-endianess) with comments and some description of meanings
 *
 * @param segment_header - struct to be print
 * @param reversed - info about big-low-endianess data type
 *                 - according specification
 */
void elf_print_segment_header(Elf32_Phdr * segment_header, uchar reversed);

/**
 * Prints consistance of array with "count" structres inside and knowing
 * about reversed order of bytes
 *
 * @param rel_array - array of Elf32_Rel structures to be printed
 * @param count - number of structures inside the array
 * @param reversed - info about big-low-endianess data type
 *                 - according specification
 */
void elf_print_rel(Elf32_Rel * rel_array, int count, int reversed);

/**
 * Prints consistance of array with "count" structres inside
 * and knowing about reversed order of bytes
 *
 * @param rela_array - array of Elf32_Rela structures to be printed
 * @param count - number of structures inside the array
 * @param reversed - info about big-low-endianess data type
 *                 - according specification
 */
void elf_print_rela(Elf32_Rela * rela_array, int count, int reversed);

/*TODO watch in code*/
void elf_print_symbol_table(int counter, char reversed,
							Elf32_Sym * symbol_table,
							char * symbol_string_table);

