#include "embox_drpbr.h"
#include "runopts.h"
#include "session.h"
#include "stdio.h"
#include <kernel/printk.h>

void dropbear_embox_store_ses() {

    struct drpbr_ses *this_ses_res = task_self_resource_dropbear();
    this_ses_res->ses_struct_addr[0] = (uintptr_t) ses;
    this_ses_res->ses_struct_addr[1] = (uintptr_t) svr_ses;
    this_ses_res->ses_struct_addr[2] = (uintptr_t) svr_opts;
    this_ses_res->ses_struct_addr[3] = (uintptr_t) opts;
}
/*Should be called only between shed_lock()/shed_unlock()*/
void dropbear_embox_switch_desc() {
    struct drpbr_ses *this_ses_res = task_self_resource_dropbear();
    ses = (struct sshsession*)this_ses_res->ses_struct_addr[0];
    svr_ses = (struct serversession*)this_ses_res->ses_struct_addr[1];
    svr_opts = (struct svr_runopts*)this_ses_res->ses_struct_addr[2];
    opts = (struct runopts*)this_ses_res->ses_struct_addr[3];
}

void dropbear_embox_free_ses(){
    struct drpbr_ses *this_ses_res = task_self_resource_dropbear();
    free((struct sshsession*)this_ses_res->ses_struct_addr[0]);
    free((struct serversession*)this_ses_res->ses_struct_addr[1]);
    free((struct svr_runopts*)this_ses_res->ses_struct_addr[2]);
    free((struct runopts*)this_ses_res->ses_struct_addr[3]);
}
