/**
 * @file
 * @brief
 *
 * @date 15.03.13
 * @author Vita Loginova
 */

#include <drivers/input/input_dev.h>
#include <drivers/video_term.h>
#include <drivers/iodev.h>

void indev_manager_init_report(const char *name){
	vterm_open_indev(&diag_vterm, name);
}
