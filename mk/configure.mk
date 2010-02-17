#
# Author: Eldar Abusalimov
#

HOSTCC  = gcc
HOSTCPP = $(HOSTCC) -E

src_confs := embox platform
mk_confs  := $(src_confs) build
lds_confs := lds

src_confs := $(src_confs:%=$(CONF_DIR)/%.conf)
mk_confs  :=  $(mk_confs:%=$(CONF_DIR)/%.conf)
lds_confs := $(lds_confs:%=$(CONF_DIR)/%.conf)

src_autoconf := $(AUTOCONF_DIR)/config.h
mk_autoconf  := $(AUTOCONF_DIR)/config.mk
lds_autoconf := $(AUTOCONF_DIR)/config.lds.h

CONF_FILES     := $(sort $(mk_confs) $(src_confs) $(lds_confs))
AUTOCONF_FILES := $(mk_autoconf) $(src_autoconf) $(lds_autoconf)

-include $(mk_autoconf)

.PHONY: check_config
check_config:
	@test -d $(CONF_DIR) $(CONF_FILES:%=-a -f %) \
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

$(mk_autoconf) : $(mk_confs)
	$(HOSTCPP) -Wp, -P -undef -nostdinc -I$(CONF_DIR) -DMAKE \
	-MMD -MT $@ -MF $@.d $(MK_DIR)/confmacro.S > $@

$(src_autoconf) : $(src_confs)
	$(HOSTCPP) -Wp, -P -undef -nostdinc -I$(CONF_DIR) \
	-MMD -MT $@ -MF $@.d $(MK_DIR)/confmacro.S \
		| sed 's/$$define/\n#define/g' | uniq > $@

$(lds_autoconf) : $(lds_confs)
	$(HOSTCPP) -Wp, -P -undef -nostdinc -I$(CONF_DIR) -DLDS \
	-MMD -MT $@ -MF $@.d $(MK_DIR)/confmacro.S \
		| sed 's/$$define/\n#define/g' | uniq > $@

$(AUTOCONF_FILES) : | mkdir # mkdir shouldn't force target to be updated

-include $(AUTOCONF_FILES:%=%.d)

mkdir:
	@test -d $(AUTOCONF_DIR) || mkdir -p $(AUTOCONF_DIR)
