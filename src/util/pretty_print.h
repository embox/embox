/**
 * @file pretty_print.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.10.2019
 */

#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H

extern size_t pretty_print_row_len(void);
extern int pretty_print_row(void *buf, size_t len, char *dest);

#endif /* PRETTY_PRINT_H */
