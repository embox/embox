/**
 * @file
 * @author Skorginskii Michal
 * @date 19.07.2010
 * @brief serial driver
 *        provides abstraction and connection between real drivers
 *        and tty drivers. It is a virtual driver with possossability
 *        of changing real driver and control of low-level I\O
 */
#include <drivers/iterminal.h>
/*
 * I/O
 */
int serial_open   ( device_t *dev , int mode ) {
	return -1;
}

int serial_close  ( device_t *dev ) {
	return -1;
}

int serial_read   ( device_t *dev , char *buf    , size_t n  ) {
	return -1;
}

int serial_write  ( device_t *dev , char *buf    , size_t n  ) {
	return -1;
}

int serial_ioctl  ( device_t *dev , io_cmd c     , void *arg ) {
	return -1;
}

int serial_devctl ( device_t *dev , device_cmd c , void *arg ) {
	switch (c) {
        // we must make choise between real dribers
	}
	return 0;
}
/*
 * register in embox as a driver
 */
int serial_load  (driver_t *drv) {
	drv->name 		= "Serial Char driver";
	drv->ops.open 	= serial_open;
	drv->ops.close 	= serial_close;
	drv->ops.read 	= serial_read;
	drv->ops.write 	= serial_write;
	drv->ops.ioctl	= serial_ioctl;
	drv->ops.devctl = serial_devctl;
	drv->flags		= 0;
	drv->private	= NULL;
	drv->private_s	= sizeof(device_t*);
	return 0;
}

/* XXX write all nessery functions for register in system -- rasmikun */
int serial_prober(driver_t *drv, void *arg) {
}

int serial_unload(driver_t *drv) {
}
/*
 * register in embox as a module (FIXME while don't exist driver's framework)
 */
static int serial_start(void) {
    //
}

static int serial_stop(void) {
    //
}

EMBOX_UNIT( serial_start, serial_stop);

#else

EMBOX_DEVICE( serial_load, serial_probe, serial_unload );
