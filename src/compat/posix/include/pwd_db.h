/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2014
 */

#ifndef PWD_GRP_DB_H_
#define PWD_GRP_DB_H_

#include <framework/mod/options.h>
#include <module/embox/compat/posix/passwd_db.h>

#define FS_DB_PREFIX __FS_DB_PREFIX

#define PASSWD_FILE  FS_DB_PREFIX "/passwd"
#define GROUP_FILE   FS_DB_PREFIX "/group"
#define SHADOW_FILE  FS_DB_PREFIX "/shadow"
#define ADDUSER_FILE FS_DB_PREFIX "/adduser.conf"


#endif /* PWD_GRP_DB_H_ */
