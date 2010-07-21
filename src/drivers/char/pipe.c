/**
 * @file
 * @author Fedor Burdun
 * @date 08.07.2010
 * @brief pipe device driver
 */

#include <embox/unit.h>
#include <kernel/driver.h>
#include <kernel/printk.h>

#define START_AS_MOD

#define BUFFER_SIZE

typedef struct pipe_private {
	char buf[BUFFER_SIZE];
	int begin, end;
} pipe_private_t;

/*
 * interface for IO
 */
int pipe_open   ( device_t *dev , int mode ) {
	return 0;
}

int pipe_close  ( device_t *dev ) {
	return 0;
}

int pipe_read   ( device_t *dev , char *buf    , size_t n  ) {
	return 0;
}

int pipe_write  ( device_t *dev , char *buf    , size_t n  ) {
	return 0;
}

int pipe_ioctl  ( device_t *dev , io_cmd c     , void *arg ) {
	return 0;
}

int pipe_devctl ( device_t *dev , device_cmd c , void *arg ) {
	return 0;
}


/*
 * interface for registry in embox as driver
 */
int pipe_load( driver_t *drv ) {
	drv->name 		= "Pipe Device Driver";
	drv->ops.open 	= pipe_open;
	drv->ops.close 	= pipe_close;
	drv->ops.read 	= pipe_read;
	drv->ops.write 	= pipe_write;
	drv->ops.ioctl	= pipe_ioctl;
	drv->ops.devctl = pipe_devctl;
	drv->flags		= 0;
	drv->private	= NULL;
	drv->private_s	= sizeof(device_t*);
	return 0;
}

int pipe_probe( driver_t *drv , void *arg ) {
	if (!arg) {
		drv->private = device_create( drv , "/dev/pipeXX" , 0 , 0 );
	} else {
		/* need interface to get device_desc */
		drv->private[drv->last ++] = device_create( drv , arg , 0 , 0 );
		/* or */
		arg = drv->private[drv->last-1];
	}
	return 0;
}

int pipe_unload( driver_t *drv ) {
	device_destroy( drv->private );
	drv->private = NULL;
	drv->private_s = 0;
	return 0;
}

/*
 * interface for registry in embox as module (while don't exist driver's framework)
 */
#ifdef START_AS_MOD
/*
 * for work need add to mods-? mods( ?.pipe , 1 ) or ?
 */
static driver_t *drv;
static driver_t drv_wm; /* without malloc */

static int pipe_start(void) {
	printk("\e[1;34mPipe driver was started!\e[0;0m\n");
	if (NULL == (drv = kmalloc( sizeof( driver_t ) )) ) {
		printk("No memory enough for start Pipe driver\n");
		return 1;
	}
	#if 0
	printf("%d\n",drv);
	pipe_load( drv );
	pipe_probe( drv , NULL );
	#else
	pipe_load( &drv_wm );
	pipe_probe( &drv_wm , NULL );
	#endif
	return 0;
}

static int pipe_stop(void) {
	pipe_unload( drv );
	kfree( drv );
	return 0;
}

EMBOX_UNIT( pipe_start , pipe_stop );

#else

EMBOX_DEVICE( pipe_load , pipe_probe , pipe_unload );

#endif

