/*
 * test_app.c
 *
 *  Created on: 18.10.2012
 *      Author: alexandr
 */

#include <xwnd/xwnd.h>
#include <xwnd/xwndapp.h>
#include <xwnd/test_app.h>

void * test_app_main (void * args) {
	struct xwnd_bmp_image * img;
	img = xwnd_bmp_load("test2.bmp");
	xwnd_bmp_draw(img);
	xwnd_bmp_unload(img);
	return NULL;
}
