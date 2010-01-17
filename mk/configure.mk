#
# Author: Eldar Abusalimov
#

HOST_CC =gcc
HOST_CPP=$(HOST_CC) -E

CONF_FILES :=$(addsuffix .conf,common drivers fs lds net tests ugly usr)

ifneq ($(MAKECMDGOALS),clean)
-include $(BUILDCONF_DIR)/config.mk
endif

.PHONY: check_config

check_config:
	@test -d $(CONF_DIR) $(addprefix -a -f $(CONF_DIR)/,$(CONF_FILES)) \
		||(echo 'Error: conf directory does not exist' \
		&& echo 'Try "make TEMPLATE=<profile> [OVERWRITE=1] config"' \
		&& echo '    See templates/ folder for possible profiles' \
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

$(addprefix $(BUILDCONF_DIR)/,config.mk config.lds.h config.h) : \
  $(MK_DIR)/confmacro.S $(addprefix $(CONF_DIR)/,$(CONF_FILES)) \
  | mkdir # mkdir shouldn't force target to be updated
	$(HOST_CPP) -Wp, -P -undef $(CPPFLAGS_$(notdir $@)) -I$(CONF_DIR) -nostdinc $< \
		| sed 's/$$define/\n#define/g' | uniq > $@

mkdir:
	@test -d $(BUILDCONF_DIR) || mkdir -p $(BUILDCONF_DIR)
