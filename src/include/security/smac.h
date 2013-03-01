/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#ifndef SECURITY_SMAC_H_
#define SECURITY_SMAC_H_

#include <stddef.h>

#define SMAC_LABELLEN 32

extern const char *smac_xattrkey;
extern const char *smac_admin;

struct smac_entry {
	char	subject[SMAC_LABELLEN];
	char 	object [SMAC_LABELLEN];
	int 	flags;
};

struct smac_env {
	int n;
	struct smac_entry entries[];
};

struct smac_task {
	char 	label[SMAC_LABELLEN];
};

extern int smac_setenv(struct smac_env *env);
extern int smac_getenv(void *buf, size_t buflen, struct smac_env **env);
extern int smac_flushenv(void);
extern int smac_addenv(const char *subject, const char *object, int flags);

extern int smac_labelset(const char *label);

extern int smac_labelget(char *label, size_t len);

extern int smac_access(const char *s_subject, const char *s_object,
		int may_access);

#endif /* SECURITY_SMAC_H_ */
