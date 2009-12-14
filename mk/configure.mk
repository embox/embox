#
# Author: Eldar Abusalimov
#

-include $(BUILD_DIR)/config.mk

.PHONY: check_config

define test-dirs
@test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
endef

check_config:
	$(test-dirs)
ifndef ARCH
	@echo 'Error: ARCH undefined.'
	exit 1
endif
ifndef TARGET
	@echo 'Error: TARGET undefined.'
	exit 1
endif

$(BUILD_DIR)/config.mk: $(MK_DIR)/confmacro.S $(wildcard $(CONF_DIR)/*.conf)
	$(test-dirs)
	gcc -DMAKE -I$(CONF_DIR) -nostdinc -E -Wp, -P $< | uniq > $@

$(BUILD_DIR)/config.lds.h: $(MK_DIR)/confmacro.S $(wildcard $(CONF_DIR)/*.conf)
	$(test-dirs)
	gcc -DLDS -I$(CONF_DIR) -nostdinc -E -Wp, -P $< \
		| sed 's/$$define/\n#define/g' | uniq > $@

$(BUILD_DIR)/config.h: $(MK_DIR)/confmacro.S $(wildcard $(CONF_DIR)/*.conf)
	$(test-dirs)
	gcc -I$(CONF_DIR) -nostdinc -E -Wp, -P $< \
		| sed 's/$$define/\n#define/g' | uniq > $@
