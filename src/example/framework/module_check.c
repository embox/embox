/**
 * @file
 *
 * @date Feb 4, 2014
 * @author: Anton Bondarev
 */
#include <stdio.h>

#include <framework/mod/api.h>

#include <framework/example/self.h>

EMBOX_EXAMPLE(run);

#define MODULE_NAME "embox.example.framework.module_check"

static int run(int argc, char **argv) {
	const struct mod *mod;

	mod = mod_lookup(MODULE_NAME);
	if (mod == NULL) {
		printf("can't find module '%s'\n", MODULE_NAME);
	}
	if (!mod_check(mod)) {
		printf("module %s is broken\n", MODULE_NAME);
	}

	if (mod->build_info && mod->build_info->label) {
		const struct mod_label *mod_label = mod->build_info->label;

		printf("module %s is correct\n", MODULE_NAME);
		printf("label is: %d, %d, %d, %d\n",
				(int)mod_label->bss.vma, (int) mod_label->data.vma,
				(int)mod_label->rodata.vma, (int)mod_label->text.vma);

		printf("check summ: %x\n", (int)mod_label->bss.vma ^ (int)mod_label->data.vma ^
				(int)mod_label->rodata.vma ^ (int)mod_label->text.vma);
	}

	return 0;
}
