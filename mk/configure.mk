#
# Author: Eldar Abusalimov
#

-include $(AUTOCONF_DIR)/config.mk

HOST_CC =gcc
HOST_CPP=$(HOST_CC) -E

#CONF_FILES :=$(addsuffix .conf,common drivers fs lds net tests ugly usr)
CONF_FILES :=$(addsuffix .conf,build ugly lds subsystems)
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

$(addprefix $(AUTOCONF_DIR)/,config.mk config.lds.h config.h) : \
  $(MK_DIR)/confmacro.S $(addprefix $(CONF_DIR)/,$(CONF_FILES)) \
  | mkdir # mkdir shouldn't force target to be updated
	$(HOST_CPP) -Wp, -P -undef $(CPPFLAGS_$(notdir $@)) -I$(CONF_DIR) -nostdinc $< \
		| sed 's/$$define/\n#define/g' | uniq > $@

mkdir:
	@test -d $(AUTOCONF_DIR) || mkdir -p $(AUTOCONF_DIR)
