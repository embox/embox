#ifndef DROPBEAR_EMBOX_DRPBR_H_
#define DROPBEAR_EMBOX_DRPBR_H

#include <kernel/sched/sched_lock.h>
#include "options.h"
#include "includes.h"
#include "runopts.h"
#include "session.h"
#include <kernel/task/resource/dropbear_res.h>


void dropbear_embox_switch_desc();
void dropbear_embox_store_ses();
void dropbear_embox_free_ses();
#endif /*DROPBEAR_EMBOX_DRPBR_H_*/
