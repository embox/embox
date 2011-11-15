
include mk/util/wildcard.mk

MY_PATH := $(ROOT_DIR)/**/Mybuild# $(ROOT_DIR)/**/*.my

MY_FILES := $(call r-wildcard,$(MY_PATH))

define printf_escape
	$(subst $(\n),\n,$(subst $(\h),\$(\h),
		$(subst \,\\,$1)
	))
endef

$(MY_FILES:$(ROOT_DIR)%Mybuild=$(EM_DIR)%Makefile) : mk/mybuild/read.mk
$(MY_FILES:$(ROOT_DIR)%Mybuild=$(EM_DIR)%Makefile) : \
		$(EM_DIR)%Makefile : $(ROOT_DIR)%Mybuild
	@mkdir -p $(@D)
	printf "%b" '$(call printf_escape,$(call gold_parse_file,myfile,$<))' > $@

