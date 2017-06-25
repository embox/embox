/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#include <kernel/host.h>
#include <hal/ipl.h>

void ipl_init(void) {
        host_ipl_init();
}

ipl_t ipl_save(void) {
        return host_ipl_save();
}

void ipl_restore(ipl_t ipl) {
        host_ipl_restore(ipl);
}
