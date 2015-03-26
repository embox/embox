/*
 * register_eh_frame.c
 *
 *  Created on: 2013-okt-03
 *      Author: fsulima
 */



#include <embox/unit.h>

#if !defined(__ARMEL__) || !(__ARMEL__==1)

extern void __embox_register_eh_frame_once (void);

static int eh_frame_init(void) {
	__embox_register_eh_frame_once();
	return 0;
}

EMBOX_UNIT_INIT(eh_frame_init);

#endif // !(__ARMEL__==1)
