#
# Embox main Makefile.
#

include mk/dirs.mk

export RM     := rm -f
export CP     := cp
export PRINTF := printf
export SHELL  := bash
export MKDIR  := mkdir -p
export LN     := ln -s
export PS1    :=

include mk/util/wildcard.mk

.PHONY: clean config xconfig menuconfig conf_update

BUILD_TARGETS := all dot docsgen

.PHONY: $(BUILD_TARGETS)

$(BUILD_TARGETS):
	@$(MAKE) -f mk/load.mk $@

clean c: _clean
	@echo 'Clean complete'

distclean dc: _distclean
	@echo 'Distclean complete'

_clean:
	@$(RM) -r $(ROOT_DIR)/build

_distclean: _clean
	@$(RM) -r $(BACKUP_DIR)
	@$(RM) -r $(CONF_DIR)


############ config ############

#build.conf assumed to be in every conf
TEMPLATES := \
	$(sort $(patsubst $(TEMPLATES_DIR)/%/build.conf,%,\
		$(call r-wildcard,$(TEMPLATES_DIR)/**/build.conf)))

$(TEMPLATES:%=confload-%) : confload-% :
	@$(CP) -fv -R -t $(CONF_DIR) $(TEMPLATES_DIR)/$*/*
	@echo 'Config complete'

ifdef TEMPLATE
ifneq ($(words $(subst /, ,$(TEMPLATE))),2)
$(error TEMPLATE must be in form PROJECT/PROFILE)
endif
override PROJECT = $(word 1,$(subst /, ,$(TEMPLATE)))
override PROFILE = $(word 2,$(subst /, ,$(TEMPLATE)))
endif

config:
	@$(MAKE) confload-$(PROJECT)/$(PROFILE)

# checking config is needed to uptate, because if it requres we must rebuild whole project
ifdef PROJECT
ifdef PROFILE

TEMPLATE_DIR = $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)

ifneq ($(call d-wildcard,$(TEMPLATE_DIR)),)
__ls_files = $(sort $(patsubst $1/%,%,$(call f-wildcard,$1/*)))
__conf_files     := $(call __ls_files,$(CONF_DIR))
__template_files := $(call __ls_files,$(TEMPLATE_DIR))

ifneq ($(call list_equal,$(__conf_files),$(__template_files)),)
conf_update : $(__conf_files:%=$(CONF_DIR)/%)
	@echo 'Config is up to date'
$(__conf_files:%=$(CONF_DIR)/%) : $(CONF_DIR)/% : $(TEMPLATE_DIR)/%
	@$(CP) -fv $< $@
else
conf_update : config
endif
endif

endif #PROFILE
endif #PROJECT

menuconfig m: DIALOG := dialog
xconfig    x: DIALOG := Xdialog

menuconfig m xconfig x:
	@TEMPLATE=`$(DIALOG) --stdout \
		--backtitle "Configuration template selection" \
		--menu "Select template to load:" 20 40 20 \
		$(TEMPLATES:%=% "" )` \
	&& $(MAKE) confload-$$TEMPLATE

