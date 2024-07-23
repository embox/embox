/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.03.2017
 */

#include <errno.h>
#include <string.h>
#include <framework/mod/api.h>
#include <framework/mod/types.h>
#include <lib/crypt/md5.h>

#include <framework/mod/integrity.h>

int mod_integrity_check(const struct mod *mod) {
	const struct mod_label *label = mod_label(mod);
	unsigned char md5[16];

	if (!label || !label->text.md5sum) {
		return -ENOTSUP;
	}

	md5_count((unsigned char *) label->text.vma, label->text.len, md5);
	if (0 != memcmp(md5, label->text.md5sum, 16)) {
		return 1;
	}

	md5_count((unsigned char *) label->rodata.vma, label->rodata.len, md5);
	if (0 != memcmp(md5, label->rodata.md5sum, 16)) {
		return 1;
	}

	return 0;
}

