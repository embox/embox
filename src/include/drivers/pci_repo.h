/**
 * @file
 *
 * @brief This manager associated vendor and device ID with according their
 *        string names
 *
 * @date 28.06.2011
 * @author Anton Bondarev
 */

#ifndef PCI_REPO_H_
#define PCI_REPO_H_

#include <types.h>

extern const char *find_vendor_name(uint16_t ven_id);

extern const char *find_device_name(uint16_t dev_id);

extern const char *find_class_name(uint8_t base, uint8_t sub);

#endif /* PCI_REPO_H_ */
