/**
 * @file
 * @brief Embox specific defines for ACPICA.
 *
 * @date 17.12.13
 * @author Roman Kurbatov
 */

#ifndef DRIVERS_ACPI_ACEMBOX_H_
#define DRIVERS_ACPI_ACEMBOX_H_

#include <limits.h>

#define ACPI_MACHINE_WIDTH WORD_BIT

#define ACPI_USE_NATIVE_DIVIDE
#define ACPI_USE_SYSTEM_CLIBRARY
#define ACPI_USE_STANDARD_HEADERS
#define ACPI_USE_LOCAL_CACHE

#endif /* DRIVERS_ACPI_ACEMBOX_H_ */
