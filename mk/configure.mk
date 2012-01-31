#
# Author: Eldar Abusalimov
#

HOSTCC  = gcc
#SVN_REV = $(shell svn info $(ROOT_DIR) | grep Rev: | awk '{print $$4}')
HOSTCPP = $(HOSTCC) -E

HOSTCC_MAJOR := \
  $(shell $(HOSTCC) -v 2>&1 | grep "gcc version" | cut -d' ' -f3  | cut -d'.' -f1)

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

include mk/mybuild/read.mk
include mk/conf/roots.mk
include mk/conf/runlevel.mk

MODS_ENABLE += embox.arch.lds
MODS_ENABLE_OBJ := \
	$(call module_closure,$(call find_mods,$(sort $(MODS_ENABLE))))

TARGET ?= embox$(if $(PLATFORM),-$(PLATFORM))
TARGET := $(TARGET)$(if $(LOCALVERSION),-$(LOCALVERSION))

.PHONY: check_config check_conf_dir
check_config: check_conf_dir $(CONF_FILES)
ifndef ARCH
	@echo 'Error: ARCH undefined'
	exit 1
endif

check_conf_dir:
	@test -d $(CONF_DIR) $(CONF_FILES:%=-a -f %) \
		||(echo 'Error: conf directory or files do not exist' \
		&& echo 'Usage: "make PROJECT=<project> PROFILE=<profile> config"' \
		&& echo '    See templates dir for possible projects and profiles' \
		&& exit 1)

$(build_mk)     : DEFS := __BUILD_MK__
$(mods_mk)      : DEFS := __MODS_MK__
$(config_h)     : DEFS := __CONFIG_H__
$(config_lds_h) : DEFS := __CONFIG_LDS_H__

$(AUTOCONF_DIR)/start_script.inc: $(CONF_DIR)/start_script.inc
	@cp -f $< $@

ifeq ($(HOSTCC_MAJOR),4)
HOSTCC_CPPFLAGS := -iquote $(CONF_DIR)
else
HOSTCC_CPPFLAGS := -I $(CONF_DIR) -I-
endif

$(build_mk) $(mods_mk) :
	$(HOSTCPP) -P -undef -nostdinc $(HOSTCC_CPPFLAGS) $(DEFS:%=-D%) \
	-MMD -MT $@ -MF $@.d $(MK_DIR)/confmacro.S \
		| sed 's/$$N/\n/g' > $@

$(config_h) $(config_lds_h) :
	$(HOSTCPP) -P -undef -nostdinc $(HOSTCC_CPPFLAGS) $(DEFS:%=-D%) \
	-MMD -MT $@ -MF $@.d $(MK_DIR)/confmacro.S \
		| sed -e 's/$$N/\n/g' -e 's/$$define/#define/g' > $@
#ifeq ($(SVN_REV),)
#	@echo "svn cmd not found"
#else
#	@echo "#define CONFIG_SVN_REV $(SVN_REV)" >> $@
#endif

$(AUTOCONF_FILES) : $(MK_DIR)/configure.mk \
  | check_conf_dir mkdir # these goals shouldn't force target to be updated

-include $(AUTOCONF_FILES:%=%.d)

CROSS_COMPILE ?=

mkdir:
	@test -d $(AUTOCONF_DIR) || mkdir -p $(AUTOCONF_DIR)
