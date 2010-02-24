#
# Author: Eldar Abusalimov
#

ifndef _depsinject_mk_
_depsinject_mk_ := 1

include $(MK_DIR)/embuild.mk

DEPSINJECT_SRC  = $(AUTOCONF_DIR)/depsinject.c
DEPSINJECT_OBJ  = $(OBJ_DIR)/depsinject.o

mod_package = $(basename $(mod))
mod_name = $(patsubst .%,%,$(suffix $(mod)))

# TODO rename this variables somehow.
dump_mods = \
  \n__embuild_mod_decl($(subst .,$$,$(mod)));
dump_mods_all = $(strip \n/* Forward declarations. */\
  $(foreach mod,$(MODS_BUILD),$(dump_mods)) \
)

dump_mod_deps = \
  \n__embuild_mod_def($(subst .,$$,$(mod)), \
    "$(mod_package)", \
    "$(mod_name)", \
    $(subst .,$$,$(DEPS-$(mod):%=\n\t__embuild_mod_p(%),)) NULL);
dump_mod_deps_all = $(strip \n/* Definitions. */\
  $(foreach mod,$(MODS_BUILD),$(dump_mod_deps)) \
)

dump_root_mods = \
  \n__embuild_mod_root($(subst .,$$,$(mod)));
dump_root_mods_all = $(strip \n/* Root modules. */\
  $(foreach mod,$(filter-out $(foreach m,$(MODS),$(DEPS-$m)),$(MODS_BUILD)), \
    $(dump_root_mods) \
  ) \
)

dump_includes = \n\#include <types.h>\n\#include <kernel/mod.h>

$(DEPSINJECT_SRC) : $(EMBUILD_DUMP_PREREQUISITES) $(MK_DIR)/depsinject.mk
	@echo -e '/* Auto-generated EMBuild deps injection file. Do not edit. */' > $@
	@echo -e '$(dump_includes)' >> $@
	@echo -e '$(dump_mods_all)' >> $@
	@echo -e '$(dump_mod_deps_all)' >> $@
	@echo -e '$(dump_root_mods_all)' >> $@

$(DEPSINJECT_OBJ) : $(DEPSINJECT_SRC)
	$(CC) $(CPPFLAGS) -D__EMBUILD_DEPSINJECT__ -o $@ -c $<

-include $(DEPSINJECT_OBJ:.o=.d)

endif
