/**
 * @file
 * @brief
 *
 * @date 22.03.13
 * @author Ilia Vaprol
 */

#ifndef LIB_B64_H_
#define LIB_B64_H_

#include <stddef.h>

extern size_t b64_coded_len(const char *plain, size_t plain_sz);
extern size_t b64_plain_len(const char *coded, size_t coded_sz);

extern int b64_encode(const char *plain, size_t plain_sz,
		char *buff, size_t buff_sz, size_t *out_coded_sz);
extern int b64_decode(const char *coded, size_t coded_sz,
		char *buff, size_t buff_sz, size_t *out_plain_sz);

#endif /* LIB_B64_H_ */
