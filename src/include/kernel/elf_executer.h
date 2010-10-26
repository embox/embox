/**
 * @file
 * @brief Describes methods to execute elf files.
 *
 * @date 15.07.2010
 * @author Avdyukhin Dmitry
 */

#ifndef ELF_EXECUTER_H_
#define ELF_EXECUTER_H_

#include <lib/elf_reader.h>

/**
 * Execute elf file.
 *
 * @param file executed file
 * @return 0 if there was no error.
 */
extern int elf_execute(FILE *file);

#endif /* ELF_EXECUTER_H_ */
