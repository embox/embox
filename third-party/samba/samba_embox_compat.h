/*
 * samba_embox_compat.h
 *
 *  Created on: 21 mars 2013
 *      Author: fsulima
 */

#ifndef SAMBA_EMBOX_COMPAT_H_
#define SAMBA_EMBOX_COMPAT_H_

#define malloc(x)         sysmalloc(x)
#define free(x)           sysfree(x)
#define memalign(x,y)     sysmemalign(x,y)
#define realloc(x,y)      sysrealloc(x,y)
#define calloc(x,y)       syscalloc(x,y)

#define rb_replace_node   rb_replace_node_samba
#define rb_prev           rb_prev_samba
#define rb_next           rb_next_samba
#define rb_first          rb_first_samba
#define rb_insert_color   rb_insert_color_samba
#define rb_erase          rb_erase_samba

#define volume_label      volume_label_samba

#endif /* SAMBA_EMBOX_COMPAT_H_ */
