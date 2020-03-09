/**
 * @file
 * @brief
 *
 * @author  Suraj Upadhyay
 * @date    05.03.2020
 */

#include <stdio.h>
#include "file_struct.h"

int ferror (FILE *file) {
    return IO_ERR_DETECT(file);
}
