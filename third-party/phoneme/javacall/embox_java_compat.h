/**
 * @file
 * @brief
 *
 * @date 7.01.13
 * @author Alexander Kalmuk
 */

#ifndef PHONEME_EMBOX_JAVA_COMPAT_H_
#define PHONEME_EMBOX_JAVA_COMPAT_H_

#include <stdlib.h>
#include <javacall_defs.h>

extern javacall_result emboxErrno2javaErrno(int embox_errno);
extern javacall_result utf16_to_utf8(const javacall_utf16* pUtf16,
                                               javacall_int32 utf16Len,
                                               unsigned char** pUtf8,
                                               javacall_int32 *utf8Len);

extern javacall_result utf8_to_utf16(javacall_utf16** pUtf16,
                                               javacall_int32 *utf16Len,
                                               const unsigned char *pUtf8,
                                               javacall_int32 utf8Len);

#endif /* PHONEME_EMBOX_JAVA_COMPAT_H_ */
