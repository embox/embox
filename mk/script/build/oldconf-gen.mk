#
#   Date: Jun 22, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

HOSTCC  = gcc
HOSTCPP = $(HOSTCC) -E

HOSTCC_VERSION := \
	$(word 3,$(shell $(HOSTCC) -v 2>&1 | grep -e "^gcc"))
HOSTCC_VERSION_MAJOR := $(word 1,$(subst ., ,$(HOSTCC_VERSION)))

ifeq ($(HOSTCC_VERSION_MAJOR),4)
HOSTCC_CPPFLAGS := -iquote $(CONF_DIR)
else
HOSTCC_CPPFLAGS := -I $(CONF_DIR) -I-
endif

build_conf   := $(CONF_DIR)/build.conf
options_conf := $(CONF_DIR)/options.conf
lds_conf     := $(CONF_DIR)/lds.conf

build_mk     := $(MKGEN_DIR)/build.mk
config_h     := $(SRCGEN_DIR)/config.h
config_lds_h := $(SRCGEN_DIR)/config.lds.h

all : $(build_mk) $(config_h) $(config_lds_h)

$(build_mk)     : $(build_conf)
$(config_h)     : $(options_conf)
$(config_lds_h) : $(lds_conf)

$(build_mk)     : DEFS := __BUILD_MK__
$(config_h)     : DEFS := __CONFIG_H__
$(config_lds_h) : DEFS := __CONFIG_LDS_H__

$(build_mk) :
	@$(call cmd_notouch_stdout,$@, \
		$(HOSTCPP) -P -undef -nostdinc $(HOSTCC_CPPFLAGS) $(DEFS:%=-D%) \
		-MMD -MP -MT $@ -MF $@.d mk/confmacro.S \
			| sed 's/\$$N/\n/g')

$(config_h) $(config_lds_h) :
	@$(call cmd_notouch_stdout,$@, \
		$(HOSTCPP) -P -undef -nostdinc $(HOSTCC_CPPFLAGS) $(DEFS:%=-D%) \
		-MMD -MT $@ -MF $@.d mk/confmacro.S \
			| sed -e 's/\$$N/\n/g' -e 's/\$$/#/g'; \
	echo '#define CONFIG_ROOTFS_IMAGE "$(ROOTFS_IMAGE)"') # XXX =/

$(AUTOCONF_DIR)/start_script.inc: $(CONF_DIR)/start_script.inc
	@$(call cmd_notouch_stdout,$@,cat $<)

-include $(addsuffix .d,$(build_mk) $(config_h) $(config_lds_h))

$(build_mk) $(config_h) $(config_lds_h): mk/script/build/oldconf-gen.mk

# XXX copy-patse
cmd_notouch = \
	set_on_error_trap() { trap "$$1" INT QUIT TERM HUP EXIT; };            \
	COMMAND=$(call sh_quote,set_on_error_trap "$(RM) $$OUTFILE"; { $2; }); \
	OUTFILE=$1;                                                            \
	if [ ! -f $$OUTFILE ];                                                 \
	then                                                                   \
		__OUTDIR=`dirname $$OUTFILE`;                                      \
		{ [ -d $$__OUTDIR ] || $(MKDIR) $$__OUTDIR; }                      \
			&& eval "$$COMMAND" && set_on_error_trap -;                    \
	else                                                                   \
		__OUTFILE=$$OUTFILE; OUTFILE=$${TMPDIR:-/tmp}/Mybuild.$$$$;        \
		eval "$$COMMAND"                                                   \
			&& { cmp -s $$OUTFILE $$__OUTFILE >/dev/null 2>&1              \
					|| { $(MV) $$OUTFILE $$__OUTFILE                       \
							&& set_on_error_trap -; }; };                  \
	fi
cmd_notouch_stdout = \
	$(call cmd_notouch,$1,{ $2; } > $$OUTFILE)
sh_quote = \
	'$(subst ','\'',$1)'

