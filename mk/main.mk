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

.PHONY: all  prepare docsgen dot clean config xconfig menuconfig conf_update

all: prepare
	@$(MAKE) -f mk/load.mk image

prepare:
	@$(MKDIR) $(BUILD_DIR)
	@$(MKDIR) $(BIN_DIR)
	@$(MKDIR) $(OBJ_DIR)
	@$(MKDIR) $(LIB_DIR)
	@$(MKDIR) $(ROOTFS_DIR)
	@$(MKDIR) $(AUTOCONF_DIR)
	@$(MKDIR) $(DOCS_OUT_DIR)

docsgen:
	@[ -d $(DOCS_OUT_DIR) ] || $(MKDIR) $(DOCS_OUT_DIR)
	doxygen
	@echo 'Docs generation complete'

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
	$(patsubst $(TEMPLATES_DIR)/%/build.conf,%,\
		$(call r-wildcard,$(TEMPLATES_DIR)/**/build.conf))

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

menuconfig m:
	@$(MAKE) confload-$$(dialog --stdout --backtitle "Configuration template selection" \
                --menu "Select template to load:" 20 40 20 \
	       	$(foreach t,$(TEMPLATES),$t "" ))
xconfig x:
	@$(MAKE) confload-$$(Xdialog --stdout --backtitle "Configuration template selection" \
                --menu "Select template to load:" 20 40 20 \
	       	$(foreach t,$(TEMPLATES),$t "" ))

