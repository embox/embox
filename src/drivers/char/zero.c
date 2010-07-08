/**
 * @file
 * @author Fedor Burdun
 * @date 08.07.2010
 * @description /dev/zero char device and example for write driver in OS "EMBOX"
 */

#include <driver.h>

#define START_AS_MOD


#ifdef START_AS_MOD
EMBOX_MOD( zero_asmod_start , zero_asmod_stop );
#else /* !START_AS_MOD */
EMBOX_DEVICE( zero_load , zero_probe , zero_unload );
#endif /* START_AS_MOD */

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
int zero_open   ( device_t dev , int mode ) {
	return 0;
}

int zero_close  ( device_t dev ) {
	return 0;
}

int zero_read   ( device_t dev , char *buf    , size_t n  ) {
#if 1
	int i;
	for ( i=0 ; i<n ; ++i ) {
		buf[i] = (char) getc();
	}
#endif
	return 0;
}

int zero_write  ( device_t dev , char *buf    , size_t n  ) {
	return 0;
}

int zero_ioctl  ( device_t dev , io_cmd c     , void *arg ) {
	return 0;
}

int zero_devctl ( device_t dev , device_cmd c , void *arg ) {
	return 0;
}


/*
 * interface for registry in embox as driver
 */
int zero_load( driver_t *drv ) {
#if 1
	drv->name 		= "Zero Device Driver";
	drv->ops.open 	= zero_open;
	drv->ops.close 	= zero_close;
	drv->ops.read 	= zero_read;
	drv->ops.write 	= zero_write;
	drv->ops.ioctl	= zero_ioctl;
	drv->ops.devctl = zero_devctl;
	drv->flags		= 0;
	drv->private	= NULL;
	drv->private_s	= sizeof(*device_t);
#endif
}

int zero_probe( driver_t *drv , void *arg ) {
#if 1
	drv->private = device_create( drv , "Zero Device" , 0 , 0 );
#endif
}

int zero_unload( driver_t *drv ) {
#if 1
	device_destroy( drv->private );
	drv->private = NULL;
	drv->private_s = 0;
#endif
}

/*
 * interface for registry in embox as module (while don't exist driver's framework)
 */
#if START_AS_MOD
/*
 * for work need add to mods-? mods( ?.zero , 1 ) or ?
 */
driver_t *drv;

static int zero_asmod_start(void) {
#if 1
	drv = kmalloc( sizeof( driver_t ) );
	zero_load( drv );
	zero_probe( drv );
#endif
}

static int zero_asmod_stop(void) {
#if 1
	zero_unload( drv );
	kfree( drv );
#endif
}
#else

