#
# Dependency Injection model generator.
#
# Author: Eldar Abusalimov
#

ifndef _codegen_di_mk_
_codegen_di_mk_ := 1

DEPSINJECT_SRC := $(CODEGEN_DIR)/depsinject.c
DEPSINJECT_OBJ := $(OBJ_DIR)/depsinject.o

generate_header := \
  /* Auto-generated EMBuild Dependency Injection model file. Do not edit. */\n

generate_includes := \n\#include <framework/mod/embuild.h>\n

__printf_escape = "$(subst ",\",$1)"
$(DEPSINJECT_SRC) : mk/codegen-di.mk $(AUTOCONF_DIR)/mods.mk
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
