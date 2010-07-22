/**
 * @file
 * @author Fedor Burdun
 * @date 19.07.2010
 * @description /dev/iterminal
 */

#include <embox/unit.h>
#include <kernel/driver.h>
#include <kernel/printk.h>
#include <shell_command.h>
#include <kernel/thread.h>
#include <drivers/iterminal.h>

#define START_AS_MOD

#define THREAD_STACK_SIZE 0x10000

static driver_t *drv;

/*
 * interface for IO
 */
int iterminal_open   ( device_t *dev , int mode ) {
	return -1;
}

int iterminal_close  ( device_t *dev ) {
	return -1;
}

int iterminal_read   ( device_t *dev , char *buf    , size_t n  ) {
	return -1;
}

int iterminal_write  ( device_t *dev , char *buf    , size_t n  ) {
	return -1;
}

int iterminal_ioctl  ( device_t *dev , io_cmd c     , void *arg ) {
	return -1;
}

int iterminal_devctl ( device_t *dev , device_cmd c , void *arg ) {
	switch (c) {
		case ITERM_DC_SET_IO: {
			((iterminal_private_t*)(dev->private))->in =
				((iterminal_private_t*)(dev->private))->out = *(int*)arg;
		} break;
		case ITERM_DC_SET_IN: {
			((iterminal_private_t*)(dev->private))->in = *(int*)arg;
		} break;
		case ITERM_DC_SET_OUT: {
			((iterminal_private_t*)(dev->private))->out = *(int*)arg;
		} break;
		case ITERM_DC_GET_IN: {
			arg = &(((iterminal_private_t*)(dev->private))->in);
		} break;
		case ITERM_DC_GET_OUT: {
			arg = &(((iterminal_private_t*)(dev->private))->out);
		} break;
		default: return DCTL_UNKNOW_OPERATION;
	}
	return 0;
}


/*
 * interface for registry in embox as driver
 */
int iterminal_load( driver_t *drv ) {
	drv->name 		= "iTerminal embox driver";
	drv->ops.open 	= iterminal_open;
	drv->ops.close 	= iterminal_close;
	drv->ops.read 	= iterminal_read;
	drv->ops.write 	= iterminal_write;
	drv->ops.ioctl	= iterminal_ioctl;
	drv->ops.devctl = iterminal_devctl;
	drv->flags		= 0;
	drv->private	= NULL;
	drv->private_s	= sizeof(device_t*);
	return 0;
}

char stack[THREAD_STACK_SIZE];
void iterminal_run(void) {
	printk("Start iterminal_run\n");
	iterminal_main( drv->private );
}

int iterminal_probe( driver_t *drv , void *arg ) {
	struct thread *th;
	drv->private = device_create( drv , "dev_itty01" , 0 , sizeof(iterminal_private_t) );
	th = thread_create( iterminal_run, stack + THREAD_STACK_SIZE );
	thread_start(th);
	return 0;
}

int iterminal_unload( driver_t *drv ) {
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
 * for work need add to mods-? mods( ?.iterminal , 1 ) or ?
 */
static int iterminal_start(void) {
	printk("\e[1;34miTerminal driver was started!\e[0;0m\n");
	if (NULL == (drv = kmalloc( sizeof( driver_t ) )) ) {
		printk("No memory enough for start iTerminal driver\n");
		return 1;
	}
	iterminal_load( drv );
	iterminal_probe( drv , NULL );
	return 0;
}

static int iterminal_stop(void) {
	iterminal_unload( drv );
	kfree( drv );
	return 0;
}

EMBOX_UNIT( iterminal_start , iterminal_stop );

#else

EMBOX_DEVICE( iterminal_load , iterminal_probe , iterminal_unload );

#endif

