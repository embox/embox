/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.05.2014
 */

#include <hal/vfork.h>


int vfork_child_start(struct task *child) {
	return -1;
}

void vfork_child_done(struct task *child, void * (*run)(void *), void *arg) {

}
