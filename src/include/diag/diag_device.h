/**
 * @file
 * @brief Diag_device interface
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#ifndef DIAG_DEVICE_H_
#define DIAG_DEVICE_H_

#include <fs/file.h>
#include <fs/ioctl.h>

extern FILE *diag_device_get(void);

#endif /* DIAG_DEVICE_H_ */
