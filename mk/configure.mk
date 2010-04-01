#
# Author: Eldar Abusalimov
#

HOSTCC  = gcc
HOSTCPP = $(HOSTCC) -E

build_conf   := $(CONF_DIR)/build.conf
options_conf := $(CONF_DIR)/options.conf
mods_conf    := $(CONF_DIR)/mods.conf
lds_conf     := $(CONF_DIR)/lds.conf

build_mk     := $(AUTOCONF_DIR)/build.mk
mods_mk      := $(AUTOCONF_DIR)/mods.mk
config_h     := $(AUTOCONF_DIR)/config.h
config_lds_h := $(AUTOCONF_DIR)/config.lds.h

CONF_FILES     := $(build_conf) $(options_conf) $(mods_conf) $(lds_conf)
AUTOCONF_FILES := $(build_mk) $(mods_mk) $(config_h) $(config_lds_h)

-include $(build_mk) $(mods_mk)

TARGET ?= embox$(if $(PLATFORM),-$(PLATFORM))

.PHONY: check_config check_conf_dir
check_config: check_conf_dir $(CONF_FILES)
ifndef ARCH
	@echo 'Error: ARCH undefined'
	exit 1
endif

check_conf_dir:
	@test -d $(CONF_DIR) $(CONF_FILES:%=-a -f %) \
		||(echo 'Error: conf directory or files do not exist' \
		&& echo 'Try "make TEMPLATE=<profile> config"' \
		&& echo '    See templates dir for possible profiles' \
		&& exit 1)

$(build_mk)     : DEFS := __BUILD_MK__
$(mods_mk)      : DEFS := __MODS_MK__
$(config_h)     : DEFS := __CONFIG_H__
$(config_lds_h) : DEFS := __CONFIG_LDS_H__

.PHONY: start_script
start_script:
	$(if $(filter %shell,$(MODS_ENABLE)),\
		$(if $(wildcard $(PATCH_CONF_DIR)/start_script.inc),\
			cp -f -t $(AUTOCONF_DIR) $(PATCH_CONF_DIR)/start_script.inc;,\
			$(if $(wildcard $(BASE_CONF_DIR)/start_script.inc),\
				cp -f -t $(AUTOCONF_DIR) $(BASE_CONF_DIR)/start_script.inc;,\
				echo 'ERROR: start_script.inc not found';\
				exit 1;\
			)\
		)\
	)

$(build_mk) $(mods_mk) :
	$(HOSTCPP) -Wp, -P -undef -nostdinc -I$(PATCH_CONF_DIR) -I$(BASE_CONF_DIR) -I- $(DEFS:%=-D%) \
	-MMD -MT $@ -MF $@.d $(MK_DIR)/confmacro.S \
		| sed 's/$$N/\n/g' > $@

$(config_h) $(config_lds_h) : start_script
	$(HOSTCPP) -Wp, -P -undef -nostdinc -I$(PATCH_CONF_DIR) -I$(BASE_CONF_DIR) -I- $(DEFS:%=-D%) \
	-MMD -MT $@ -MF $@.d $(MK_DIR)/confmacro.S \
		| sed 's/$$N/\n/g' | sed 's/$$define/#define/g' > $@

$(AUTOCONF_FILES) : $(MK_DIR)/configure.mk \
  | mkdir # this goal shouldn't force target to be updated

-include $(AUTOCONF_FILES:%=%.d)

mkdir:
	@test -d $(AUTOCONF_DIR) || mkdir -p $(AUTOCONF_DIR)
