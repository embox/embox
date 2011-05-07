/**
 * @file
 * @brief /dev/zero char device and example for write driver
 *
 * @date 08.07.2010
 * @author Fedor Burdun
 */

#include <embox/unit.h>
#include <kernel/driver.h>
#include <kernel/printk.h>
#include <kernel/mm/kmalloc.h>

#define START_AS_MOD

/*
 * inner function
 */
int getc(void) {
	return '0';
}

int putc(int v) {
	return 0;
}

/*
 * interface for IO
 */
int zero_open(device_t *dev, int mode) {
	return 0;
}

int zero_close(device_t *dev) {
	return 0;
}

int zero_read(device_t *dev, char *buf, size_t n) {
	size_t i;
	for (i = 0; i < n; ++i) {
		buf[i] = (char) getc();
	}
	return 0;
}

int zero_write(device_t *dev, char *buf, size_t n) {
	return 0;
}

int zero_ioctl(device_t *dev, io_cmd c, void *arg) {
	return 0;
}

int zero_devctl(device_t *dev, device_cmd c, void *arg) {
	return 0;
}

/*
 * interface for registry in Embox as driver
 */
int zero_load(driver_t *drv) {
	drv->name       = "Zero Device Driver";
	drv->ops.open   = zero_open;
	drv->ops.close  = zero_close;
	drv->ops.read   = zero_read;
	drv->ops.write  = zero_write;
	drv->ops.ioctl  = zero_ioctl;
	drv->ops.devctl = zero_devctl;
	drv->flags      = 0;
	drv->private    = NULL;
	drv->private_s  = sizeof(device_t*);
	return 0;
}

int zero_probe(driver_t *drv, void *arg) {
	drv->private = device_create(drv, "dev_zero", 0, 0);
	return 0;
}

int zero_unload(driver_t *drv) {
	device_destroy(drv->private);
	drv->private = NULL;
	drv->private_s = 0;
	return 0;
}

/*
 * interface for registry in Embox as module (while don't exist driver's framework)
 */
#ifdef START_AS_MOD
/*
 * for work need add to mods-? mods(?.zero, 1) or ?
 */
static driver_t *drv;
//static driver_t drv_wm; /* without malloc */

static int zero_start(void) {
	printk("\033[1;34mZero driver was started!\033[0;0m\n");

	if (NULL == (drv = kmalloc(sizeof(driver_t)))) {
		printk("No memory enough for start Zero driver\n");
		return 1;
	}
	zero_load(drv);
	zero_probe(drv, NULL);
	return 0;
}

static int zero_stop(void) {
	zero_unload(drv);
	kfree(drv);
	return 0;
}

EMBOX_UNIT(zero_start, zero_stop);

#else

EMBOX_DEVICE(zero_load, zero_probe, zero_unload);

#endif
