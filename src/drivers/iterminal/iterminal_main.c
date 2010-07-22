/**
 * @file
 * @author Fedor Burdun
 * @date 19.07.2010
 * @brief terminal's driver
 */

#include <kernel/driver.h>
#include <kernel/printk.h>
#include <stdio.h>
#include <drivers/iterminal.h>
#include "iterminal_main.c"


#define FROM_IN 0x01
#define FROM_OUT 0x02

#define IN (((iterminal_private_t*)(dev->private))->in)
#define OUT (((iterminal_private_t*)(dev->private))->out)

#define PRIVATE(dev) ((iterminal_private_t*)(dev->private))

inline void iterminal_main_init( device_t *dev , int from_flag ) {
	switch ( from_flag ) {
		case FROM_IN:
		/* try to switch non-blocking mode input device */
		break;

		case FROM_OUT:
		/* try to switch non-blocking mode output device */
		break;

		default:
		break;
	}
	return ;
}

inline device_desc in( device_t *dev ) {
	if ( IN == PRIVATE(dev)->before_in ) {
		return IN;
	} else {
		iterminal_main_init( dev , FROM_IN );
		return PRIVATE(dev)->before_in = IN;
	}
}

inline device_desc out( device_t *dev ) {
	if ( OUT == PRIVATE(dev)->before_out ) {
		return OUT;
	} else {
		iterminal_main_init( dev , FROM_OUT );
		return PRIVATE(dev)->before_out = OUT;
	}
}

inline int has_next_token( const char *buf , int cur_pos ) {
	return 0;
}

inline int next_token( const char *buf, int *cur_pos ) {
	return 0;
}

#if 0
int call( device_t *dev , int token , char *out_buffer /* return it, int out_buffer_s */) {
	if ( ! PRIVATE(dev)->call_state ) { /* cmd line? */
		switch ( token ) {
			/*
			 * some magic switch, that may be copied from previous shell
			 */

			#if 0x0
			case TOKEN_ENTER:
				/* change call_state */
				/* create 2 pipe device
				 	pipe_probe?
				*/
				/* set stdio context */
				/* run job in new thread */
			break;
			#endif

			default:
			/*
			 * write to out_buffer as is
			 */
             dev=
			break;
		}
	} else {
		switch ( token ) {
			/*
			 * some switch, that pick out only ^C like tokens
			 *    ^C, ^S, ^Q
			 */
			#if 0x0
			case TOKEN_SIGINT: /* ^C */
				/* change call_state */
				/* kill thread */
				/* destroy pipe devices */
			break;
			#endif
		}
	}
	return 0;
}
#endif

int iterminal_main( device_t *dev ) {

	printk("iterminal_main\n");

	while ( PRIVATE(dev)->is_live ) {
		/* some code for read from in(dev)
		 *
		 * device_read( in(dev) , PRIVATE(dev)->buffer_in , PRIVATE(dev)->buffer_in_s );
		 *
		 */
		int pos = 0;

		while ( has_next_token( PRIVATE(dev)->buffer_in , pos ) ) {
			/*
			 * general work
			 */
			PRIVATE(dev)->buffer_out_s = call( dev , next_token( PRIVATE(dev)->buffer_in , &pos ) ,
				PRIVATE(dev)->buffer_out );
			/*
			 * output, that may be below ( outside the cycle )
			 *
			 * device_write ( out(dev) , PRIVATE(dev)->buffer_out , PRIVATE(dev)->buffer_out_s );
			 *
			 */
		}
	}


	/* shell_start(); */

#if 0
	while (true) {
		char ch = '\0';
		if (device_read( in(dev) , &ch , 1 ) || 1) {
			device_write( out(dev) , &ch , 1 );
			printk("iterminal: %d\n", (int)ch);
		} else {
		}
	}
#endif

	return 0;
}

