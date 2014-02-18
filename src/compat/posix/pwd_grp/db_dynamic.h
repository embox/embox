/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2014
 */

#ifndef PWD_GRP_DB_DYNAMIC_H_
#define PWD_GRP_DB_DYNAMIC_H_

#include <framework/mod/options.h>
#include <module/embox/compat/posix/passwd_db.h>

#define __FS_DB_PREFIX OPTION_MODULE_STRING_GET(embox__compat__posix__passwd_db_dynamic, db_path)

#endif /* PWD_GRP_DB_DYNAMIC_H_ */

