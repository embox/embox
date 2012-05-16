#
# Dependency Injection model generator.
#
# Author: Eldar Abusalimov
#

ifndef _codegen_di_mk_
_codegen_di_mk_ := 1

DEPSINJECT_SRC := $(SRCGEN_DIR)/depsinject.c
DEPSINJECT_OBJ := $(OBJ_DIR)/depsinject.o

$(DEPSINJECT_SRC) : mk/codegen-di.mk $(configfiles_model_mk)
	$(MAKE) depsinject=1 -f mk/script/depsinject.mk > $@

$(DEPSINJECT_OBJ) : $(AUTOCONF_DIR)/config.h
$(DEPSINJECT_OBJ) : $(DEPSINJECT_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -std=gnu99 -D__EMBUILD__ -o $@ -c $<

-include $(DEPSINJECT_OBJ:.o=.d)

endif
