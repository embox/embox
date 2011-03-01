/**
 * @file
 * @brief pipe device driver
 *
 * @author Fedor Burdun
 * @date 22.07.2010
 */

#include <embox/unit.h>
#include <kernel/driver.h>
#include <kernel/printk.h>
#include <kernel/mm/kmalloc.h>

#define START_AS_MOD

#define MAX_BUFFER_SIZE 0x100

#define __B_MIN(a, b)      ( ( (a)<(b) ) ? (a) : (b) )
#define __B_MAX(a, b)      ( ( (a)<(b) ) ? (b) : (a) )

#define PRIVATE(dev)          ((pipe_private_t*)(dev->private))
#define BEG(dev)              ( PRIVATE(dev)->buf_begin )
#define END(dev)              ( PRIVATE(dev)->buf_end )

#define BUFFER(dev)           ( PRIVATE(dev)->buf )
#define BUFFER_IS_EMPTY(dev)  ( BEG(dev) == END(dev) )
#define BUFFER_SIZE(dev)      ( ( END(dev) - BEG(dev) + MAX_BUFFER_SIZE ) % MAX_BUFFER_SIZE )
#define BUFFER_HAS_SPACE(dev) ( ( BEG(dev) - END(dev) + MAX_BUFFER_SIZE ) % MAX_BUFFER_SIZE )
#define BUFFER_IS_FULL(dev)   ( BUFFER_HAS_SPACE(dev) == 1 )

#define BUFFER_LENGTH_FP(dev) __B_MIN(( BEG(dev) != 0) ? \
	(MAX_BUFFER_SIZE - END(dev)) :     \
	(MAX_BUFFER_SIZE - END(dev) - 1) , \
	__B_MAX( BEG(dev) - END(dev) - 1 , 0 ) ) /* First part */

#define BUFFER_LENGTH_SC(dev)	( BEG(dev) < END(dev) ? \
	( BEG(dev) - 1) : ( 0 ) ) /* Second part */

#define BUFFER_PUSH(dev,_char_)	( BUFFER(dev) [ END(dev)++ ] = _char_ )
#define BUFFER_POP(dev)	        ( BUFFER(dev) [ BEG(dev)++ ] )
#define BUFFER_CUR_CH(dev) \
	do { if (BEG(dev) == MAX_BUFFER_SIZE) { BEG(dev) = 0; }    \
		if (END(dev) == MAX_BUFFER_SIZE) { END(dev) = 0; } \
	} while (0);

typedef struct pipe_private {
	int ioctl_base_flags;
	char buf[MAX_BUFFER_SIZE];
	int buf_begin, buf_end;
} pipe_private_t;

/*
 * interface for IO
 */
int pipe_open(device_t *dev, int mode) {
	return 0;
}

int pipe_close(device_t *dev) {
	return 0;
}

int pipe_read(device_t *dev, char *buf, size_t n) {
	size_t i;
	if (PRIVATE(dev)->ioctl_base_flags | IOCTLP_NONBLOCKINGIO) {
		/* non-blocking read (may read less chars than n)
		 * return how many bytes was read */
		for (i = 0; i < n && !BUFFER_IS_EMPTY(dev); ++i) {
			buf[i] = BUFFER_POP(dev);
			BUFFER_CUR_CH(dev);
		}
		return i;
	} else {
		if (n >= BUFFER_SIZE(dev)) {
			return -ERROR_IO_BUFSIZE;
		}
		/* blocking read */
		for (i = 0; i < n && !BUFFER_IS_EMPTY(dev); ++i) {
			buf[i] = BUFFER_POP(dev);
			BUFFER_CUR_CH(dev);
		}
		return 0;
	}
}

int pipe_write(device_t *dev, char *buf, size_t n) {
	size_t i;
	if (PRIVATE(dev)->ioctl_base_flags | IOCTLP_NONBLOCKINGIO) {
		/* non-blocking write (may write only part)
		 * return how many bytes was written*/

		for (i = 0; i < n && !BUFFER_IS_FULL(dev); ++i) {
			BUFFER_PUSH( dev, buf[i] );
			BUFFER_CUR_CH(dev);
		}
		return i;
#if 0
		int fc = __B_MIN( BUFFER_LENGTH_FP(dev), n); /* first cycle */

		for (i = 0; i < fc; ++i) {
			BUFFER_PUSH(dev, buf[i]);
		}
		BUFFER_CUR_CH(dev);
		for ( ; i < n && !BUFFER_IS_EMPTY(dev); ++i ) {
			BUFFER_PUSH(dev, buf[i]);
		}
		return i;
#endif

	} else { /* blocking write (write only all) */
		if (n >= BUFFER_HAS_SPACE(dev)) {
			return -ERROR_IO_BUFSIZE;
		}

		for ( i = 0; i < n && !BUFFER_IS_FULL(dev); ++i) {
			BUFFER_PUSH(dev, buf[i]);
			BUFFER_CUR_CH(dev);
		}
		return 0;
#if 0
		int fc = __B_MIN(BUFFER_LENGTH_FP(dev), n); /* first cycle */

		for (i = 0; i < fc; ++i) {
			BUFFER_PUSH(dev, buf[i]);
		}
		BUFFER_CUR_CH(dev);
		for ( ; i < n; ++i) {
			BUFFER_PUSH(dev, buf[i]);
		}
		return 0;
#endif
	}
}

int pipe_ioctl(device_t *dev, io_cmd c, void *arg) {
	switch (c) {
	case IOCTL_SET_BASE_OPTIONS:
		PRIVATE(dev)->ioctl_base_flags = *((int*) arg);
		break;
	case IOCTL_GET_BASE_OPTIONS:
		arg = &(PRIVATE(dev)->ioctl_base_flags);
		break;
	default:
		return IOCTLR_UNKNOW_OPERATION;
	}
	return IOCTLR_OK;
}

int pipe_devctl(device_t *dev, device_cmd c, void *arg) {
	return 0;
}


/*
 * interface for registry in Embox as driver
 */
int pipe_load(driver_t *drv) {
	drv->name       = "Pipe Device Driver";
	drv->ops.open   = pipe_open;
	drv->ops.close  = pipe_close;
	drv->ops.read   = pipe_read;
	drv->ops.write  = pipe_write;
	drv->ops.ioctl  = pipe_ioctl;
	drv->ops.devctl = pipe_devctl;
	drv->flags      = 0;
	drv->private    = NULL;
	drv->private_s  = sizeof(device_t*);
	return 0;
}

int pipe_probe(driver_t *drv, void *arg) {
	if (!arg) {
		drv->private = device_create(drv, "/dev/pipeXX", 0, 0);
	} else {
		/* need interface to get device_desc */
		/* drv->private[drv->last++] = device_create(drv, arg, 0, 0); */
		device_create(drv, arg, 0, 0);
		/* or */
		/* arg = drv->private[drv->last-1]; */
	}
	return 0;
}

int pipe_unload(driver_t *drv) {
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
 * for work need add to mods-? mods( ?.pipe , 1 ) or ?
 */
static driver_t *drv;

static int pipe_start(void) {
	printk("\033[1;34mPipe driver was started!\033[0;0m\n");
	if (NULL == (drv = kmalloc(sizeof(driver_t)))) {
		printk("No memory enough for start Pipe driver\n");
		return 1;
	}
	pipe_load(drv);
	pipe_probe(drv, NULL);
	return 0;
}

static int pipe_stop(void) {
	pipe_unload(drv);
	kfree(drv);
	return 0;
}

EMBOX_UNIT(pipe_start, pipe_stop);

#else

EMBOX_DEVICE(pipe_load, pipe_probe, pipe_unload);

#endif

