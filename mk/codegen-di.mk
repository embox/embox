#
# Dependency Injection model generator.
#
# Author: Eldar Abusalimov
#

ifndef _codegen_di_mk_
_codegen_di_mk_ := 1

include $(MK_DIR)/embuild.mk

DEPSINJECT_SRC = $(CODEGEN_DIR)/depsinject.c
DEPSINJECT_OBJ = $(OBJ_DIR)/depsinject.o

mod_package = $(basename $(mod))
mod_name = $(patsubst .%,%,$(suffix $(mod)))

# The same as DEPS-$(mod), at least for now.
mod_requires = $(DEPS-$(mod))
# This is very inefficient algorithm but it does work.
mod_provides = $(filter-out $(mod),$(foreach m,$(MODS_BUILD), \
  $(if $(filter $(mod),$(call MOD_DEPS_DAG,$(m))),$(m)) \
))

c_mod = $(call c_escape,$(mod))
c_dep = $(call c_escape,$(dep))
c_package = $(call c_escape,$(package))

c_escape = $(subst .,$$,$(1))

generate_package_defs = $(strip \n/* Package definitions. */\
  $(foreach package,$(filter-out generic,$(sort $(basename $(MODS_BUILD)))), \
    \nMOD_PACKAGE_DEF($(c_package), "$(package)"); \
  ) \
)

generate_mod_defs = $(strip \n/* Mod definitions. */\
  $(foreach mod,$(MODS_BUILD), \
    \nMOD_DEF($(c_mod), $(call c_escape,$(mod_package)), "$(mod_name)"); \
  ) \
)

#__generate_mod_deps = \
  \n$1($(c_mod),$(call c_escape,$(foreach dep,$2,\n\tMOD_PTR($(dep)),)) NULL);
#    $(call __generate_mod_deps,MOD_REQUIRES_DEF,$(mod_requires)) \
#    $(call __generate_mod_deps,MOD_PROVIDES_DEF,$(mod_provides)) \

generate_mod_deps = $(strip \n/* Mod deps. */\
  $(foreach mod,$(MODS_BUILD), \
    $(foreach dep,$(DEPS-$(mod)), \
      \nMOD_DEP_DEF($(c_mod), $(c_dep)); \
    ) \
    $(if $(RUNLEVEL-$(mod)), \
      \nMOD_DEP_DEF(runlevel$(RUNLEVEL-$(mod))_init, $(c_mod)); \
      \nMOD_DEP_DEF($(c_mod), runlevel$(RUNLEVEL-$(mod))_fini); \
    ) \
  ) \
)

generate_root_mods = $(strip \n/* Root modules. */\
  $(foreach mod,$(filter-out $(foreach m,$(MODS_BUILD),$(DEPS-$m)),$(MODS_BUILD)), \
    \nMOD_ROOT_DEF($(subst .,$$,$(mod))); \
  ) \
)

generate_includes = \n\#include <types.h>\n\#include <embox/mod.h>

$(DEPSINJECT_SRC) : $(EMBUILD_DUMP_PREREQUISITES) $(MK_DIR)/codegen-di.mk \
  $(AUTOCONF_DIR)/mods.mk
	@echo -e '/* Auto-generated EMBuild deps injection file. Do not edit. */' > $@
	@echo -e '$(generate_includes)' >> $@
	@echo -e '$(generate_package_defs)' >> $@
	@echo -e '$(generate_mod_defs)' >> $@
	@echo -e '$(generate_mod_deps)' >> $@
	@echo -e '$(generate_root_mods)' >> $@

$(DEPSINJECT_OBJ) : $(AUTOCONF_DIR)/config.h
$(DEPSINJECT_OBJ) : $(DEPSINJECT_SRC)
	$(CC) $(CPPFLAGS) -D__EMBUILD_DEPSINJECT__ -o $@ -c $<

-include $(DEPSINJECT_OBJ:.o=.d)

endif
