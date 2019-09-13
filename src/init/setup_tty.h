/**
 * @file
 * @brief
 *
 * @date 06.03.14
 * @author Ilia Vaprol
 */

#ifndef SETUP_TTY_H_
#define SETUP_TTY_H_

#include <module/embox/init/setup_tty_api.h>

#define DIAG_NAME "diag"
extern int diag_fd(void);

extern const char *setup_tty(const char *dev_name);

#endif /* SETUP_TTY_H_ */
