#
# Author: Eldar Abusalimov
#

-include $(AUTOCONF_DIR)/config.mk

HOSTCC  = gcc
HOSTCPP = $(HOSTCC) -E

CONF_FILES := $(addsuffix .conf,build lds platform embox)
AUTOCONF_FILES := config.mk config.lds.h config.h

.PHONY: check_config
check_config:
	@test -d $(CONF_DIR) $(addprefix -a -f $(CONF_DIR)/,$(CONF_FILES)) \
		||(echo 'Error: conf directory does not exist' \
		&& echo 'Try "make TEMPLATE=<profile> config"' \
		&& echo '    See templates dir for possible profiles' \
		&& exit 1)
ifndef ARCH
	@echo 'Error: ARCH undefined'
	exit 1
endif
ifndef TARGET
	@echo 'Error: TARGET undefined'
	exit 1
endif

CPPFLAGS_config.mk   :=-DMAKE
CPPFLAGS_config.lds.h:=-DLDS
CPPFLAGS_config.h    :=

$(addprefix $(AUTOCONF_DIR)/,$(AUTOCONF_FILES)) : \
  $(MK_DIR)/confmacro.S $(addprefix $(CONF_DIR)/,$(CONF_FILES)) \
  | mkdir # mkdir shouldn't force target to be updated
	$(HOSTCPP) -Wp, -P -undef $(CPPFLAGS_$(notdir $@)) -I$(CONF_DIR) -nostdinc \
	-MMD -MT $@ -MF $@.d $< \
		| sed 's/$$define/\n#define/g' | uniq > $@

-include $(AUTOCONF_FILES:%=$(AUTOCONF_DIR)/%.d)

mkdir:
	@test -d $(AUTOCONF_DIR) || mkdir -p $(AUTOCONF_DIR)
