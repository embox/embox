#
#   Date: Jun 22, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

HOSTCC  = cc
HOSTCPP = $(HOSTCC) -E

HOSTCC_CPPFLAGS := -iquote $(CONF_DIR)

build_conf   := $(CONF_DIR)/build.conf
lds_conf     := $(CONF_DIR)/lds.conf

build_mk     := $(MKGEN_DIR)/build.mk
config_lds_h := $(SRCGEN_DIR)/config.lds.h

regions_lds_h  := $(SRCGEN_DIR)/regions.lds.h
sections_lds_h := $(SRCGEN_DIR)/sections.lds.h
phdrs_lds_h    := $(SRCGEN_DIR)/phdrs.lds.h

all : $(build_mk) $(config_lds_h) $(regions_lds_h) \
	$(sections_lds_h) $(phdrs_lds_h)

$(build_mk)     : $(build_conf)
$(config_lds_h) : $(lds_conf)

$(build_mk)     : DEFS := __BUILD_MK__
$(config_lds_h) : DEFS := __CONFIG_LDS_H__

$(build_mk) :
	@$(call cmd_notouch_stdout,$@, \
		$(HOSTCPP) -P -undef -nostdinc $(HOSTCC_CPPFLAGS) $(DEFS:%=-D%) \
		-MMD -MP -MT $@ -MF $@.d mk/confmacro.S \
			| $(AWK) '{ gsub("\\$$N","\n"); print }')

gen_regions = \
	$(abspath $(ROOT_DIR))/mk/gen_ld_regions.sh $1 $2

gen_sections = \
	$(abspath $(ROOT_DIR))/mk/gen_ld_sections.sh $1 $2

gen_phdrs = \
	$(abspath $(ROOT_DIR))/mk/gen_ld_phdrs.sh $1 $2

$(config_lds_h) :
	@$(call cmd_notouch_stdout,$@, \
		$(HOSTCPP) -P -undef -nostdinc $(HOSTCC_CPPFLAGS) $(DEFS:%=-D%) \
		-MMD -MT $@ -MF $@.d mk/confmacro.S \
			| $(AWK) '{ gsub("\\$$N","\n"); gsub("\\$$","#"); print }')

$(regions_lds_h) : $(config_lds_h)
	@$(call gen_regions, $(config_lds_h), $@)

$(sections_lds_h) : $(config_lds_h)
	@$(call gen_sections, $(config_lds_h), $@)

$(phdrs_lds_h) : $(config_lds_h)
	@$(call gen_phdrs, $(config_lds_h), $@)

$(AUTOCONF_DIR)/start_script.inc: $(CONF_DIR)/start_script.inc
	@$(call cmd_notouch_stdout,$@,cat $<)

-include $(addsuffix .d,$(build_mk) $(config_lds_h))

%/. :
	@$(MKDIR) $*

.SECONDEXPANSION :
$(build_mk) $(config_lds_h) : mk/script/build/oldconf-gen.mk | $$(@D)/.

