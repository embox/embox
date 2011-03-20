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

c_escape = $(subst .,__,$(1))

c_str_escape = \
  \n\t\t"$(subst $(\n),\\\\n"\n\t\t",$(subst $(\t),\\t,$(subst ",\",$1)))"

eol-trim = $(if $(findstring $() \n,$1),$(call $0,$(subst $() \n,\n,$1)),$1)

cond_flags =   $(if $(strip $(CFLAGS-$2) $(CPPFLAGS-$2)), \
    $1 $(strip $(CFLAGS-$2) $(CPPFLAGS-$2)) \
  ) \

package_def = \
  \n\n/* \
  \n * Package: $(package) \
  $(call cond_flags,\n *   FLAGS:,$(package)) \
  \n */ \
  \nMOD_PACKAGE_DEF($(c_package), "$(package)");

generate_package_defs = $(call eol-trim,\n/* Package definitions. */\
  $(foreach package,$(sort generic $(basename $(MODS_BUILD))), \
    $(package_def) \
  ) \
)\n

mod_def = \
  \n\n/* \
  \n * Mod: $(mod) \
  $(call cond_flags,\n *   FLAGS:,$(mod)) \
  \n * Sources: \
  $(foreach src,$(SRCS-$(mod)), \
    \n *    $(src) \
    $(call cond_flags,\n *       FLAGS:,$(abspath $(src))) \
  ) \
  \n */ \
  \nMOD_DEF($(c_mod), $(call c_escape,$(mod_package)), "$(mod_name)", \
    $(call c_str_escape,$(BRIEF-$(mod))), \
    $(call c_str_escape,$(DETAILS-$(mod))));

generate_mod_defs = $(call eol-trim,\n/* Mod definitions. */\
  $(foreach mod,$(MODS_BUILD), \
    $(mod_def) \
  ) \
)\n

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
)\n

generate_header = \
  /* Auto-generated EMBuild Dependency Injection model file. Do not edit. */\n

generate_includes = \n\#include <framework/mod/embuild.h>\n

__printf_escape = "$(subst ",\",$1)"
$(DEPSINJECT_SRC) : $(EMBUILD_DUMP_PREREQUISITES) $(MK_DIR)/codegen-di.mk \
  $(AUTOCONF_DIR)/mods.mk
	@$(PRINTF) $(call __printf_escape,$(generate_header)) > $@
	@$(PRINTF) $(call __printf_escape,$(generate_includes)) >> $@
	@$(PRINTF) $(call __printf_escape,$(generate_package_defs)) >> $@
	@$(PRINTF) $(call __printf_escape,$(generate_mod_defs)) >> $@
	@$(PRINTF) $(call __printf_escape,$(generate_mod_deps)) >> $@

$(DEPSINJECT_OBJ) : $(AUTOCONF_DIR)/config.h
$(DEPSINJECT_OBJ) : $(DEPSINJECT_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -std=gnu99 -D__EMBUILD__ -o $@ -c $<

-include $(DEPSINJECT_OBJ:.o=.d)

endif
