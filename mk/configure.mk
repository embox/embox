#
# Author: Eldar Abusalimov
#

CONF_FILES :=$(addsuffix .conf,common drivers fs lds net tests ugly usr)

-include $(BUILD_DIR)/config.mk

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

CCFLAGS_config.mk   :=-DMAKE
CCFLAGS_config.lds.h:=-DLDS
CCFLAGS_config.h    :=

$(addprefix $(BUILD_DIR)/,config.mk config.lds.h config.h) : \
  $(MK_DIR)/confmacro.S $(addprefix $(CONF_DIR)/,$(CONF_FILES)) \
  | mkdir # mkdir shouldn't force target to be updated
	gcc $(CCFLAGS_$(notdir $@)) -I$(CONF_DIR) -nostdinc -undef -E -Wp, -P $< \
		| sed 's/$$define/\n#define/g' | uniq > $@

mkdir:
	@test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
