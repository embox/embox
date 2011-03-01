/**
 * @file
 * @brief /dev/iterminal
 * @decription In fact, this is tty driver, that provides interfaces
 * for shell developer. It shoud provide such opportunitys:
 *  - read and write a simple string
 *  - XXX think what opportunities must provide the tty driver -- rasmikun
 *
 * @date 19.07.201
 * @author Fedor Burdun
 * @auther Michail Skorginskii
 *         - I made some simple refactor - renaming,
 *           delete and some part of code, wich i can't understad.
 *           We make simple terminal, right?
 */

#include <embox/unit.h>
#include <kernel/driver.h>
#include <kernel/printk.h>
#include <shell_command.h>
#include <kernel/thread.h>
#include <drivers/iterminal.h>
#include <drivers/vtbuild.h>
#include <drivers/vtparse.h>

#define START_AS_MOD

#define THREAD_STACK_SIZE 0x10000

static driver_t *drv;

/*
 * interface for IO
 */
int iterminal_open(device_t *dev, int mode) {
	return -1;
}

int iterminal_close(device_t *dev) {
	return -1;
}

int iterminal_read(device_t *dev, char *buf, size_t n) {
	return -1;
}

int iterminal_write(device_t *dev, char *buf, size_t n) {
	return -1;
}

int iterminal_ioctl(device_t *dev, io_cmd c, void *arg) {
	return -1;
}

int iterminal_devctl(device_t *dev, device_cmd c, void *arg) {
	switch (c) {
    		/* We must decide how we can rule the ttu device TODO -- rasmikun
    		*/
	}
	return 0;
}

/*
 * interface for registry in embox as driver
 */
int iterminal_load(driver_t *drv) {
	drv->name       = "iTerminal driver";
	drv->ops.open   = iterminal_open;
	drv->ops.close  = iterminal_close;
	drv->ops.read   = iterminal_read;
	drv->ops.write  = iterminal_write;
	drv->ops.ioctl  = iterminal_ioctl;
	drv->ops.devctl = iterminal_devctl;
	drv->flags      = 0;
	drv->private    = NULL;
	drv->private_s  = sizeof(device_t*);
	return 0;
}

char stack[THREAD_STACK_SIZE];

void iterminal_run(void) {
	printk("Start iterminal_run\n");
	iterminal_main(drv->private);
}

int iterminal_probe(driver_t *drv, void *arg) {
	struct thread *th;
	drv->private = device_create(drv, "dev_itty01", 0,
				    sizeof(iterminal_private_t));
	th = thread_create(iterminal_run, stack + THREAD_STACK_SIZE);
	thread_start(th);
	return 0;
}

int iterminal_unload(driver_t *drv) {
	device_destroy(drv->private);
	drv->private = NULL;
	drv->private_s = 0;
	return 0;
}

/*
 * interface for registry in Embox as module (FIXME while don't exist driver's framework)
 *
 * FIXME rewrite all thease functions for their real need's
 */
#ifdef START_AS_MOD
/*
 * for work need add to mods-? mods(?.iterminal, 1) or ?
 */
static int iterminal_start(void) {
	printk("\e[1;34miTerminal driver was started!\e[0;0m\n");
	if (NULL == (drv = kmalloc(sizeof(driver_t)))) {
		printk("No memory enough for start iTerminal driver\n");
		return 1;
	}
	iterminal_load(drv);
	iterminal_probe(drv, NULL);
	return 0;
}

static int iterminal_stop(void) {
	iterminal_unload(drv);
	kfree(drv);
	return 0;
}

EMBOX_UNIT(iterminal_start, iterminal_stop);

#else

EMBOX_DEVICE(iterminal_load, iterminal_probe, iterminal_unload);

#endif

