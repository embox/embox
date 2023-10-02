/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    24.02.2014
 */

#ifndef USER_H_
#define USER_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <pwd_db.h>

#define BUF_LEN 64

#define TMP_PASSWD_FILE "/tmp/passwd.temp"
#define TMP_SHADOW_FILE "/tmp/shadow.temp"

extern void user_put_string(char*, FILE*, char);
extern void user_put_int(int, FILE*, char);
extern int user_is_user_exists(char*);
extern int user_get_group(char*);
extern void user_write_user_passwd(struct passwd*, FILE*);
extern void user_write_user_spwd(struct spwd*, FILE*);
extern int user_set_options_passwd(struct passwd*, char*, char*, char*, int);
extern int user_set_options_spwd(struct spwd*, char*, char*);
extern int user_copy(const char*, const char*);

#endif /* USER_H_ */
