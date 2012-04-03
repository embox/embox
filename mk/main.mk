#
# Embox main Makefile.
#

include mk/dirs.mk

MY_PATH := $(SRC_DIR)/** $(PLATFORM_DIR)/** $(THIRDPARTY_DIR)/**

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
	$(MAKE) -f mk/load.mk check_config image
	@echo 'Build complete'

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

ifdef TEMPLATE
ifneq ($(words $(subst /, ,$(TEMPLATE))),2)
$(error TEMPLATE must be in form PROJECT/PROFILE)
endif
override PROJECT = $(word 1,$(subst /, ,$(TEMPLATE)))
override PROFILE = $(word 2,$(subst /, ,$(TEMPLATE)))
endif

config: _clean
ifndef PROFILE
	@echo 'Error: PROFILE undefined'
	@echo 'Usage: "make PROJECT=<project> PROFILE=<profile> config"'
	@echo '    See templates dir for possible projects and profiles'
	exit 1
endif
ifndef PROJECT
	@echo 'Error: PROJECT undefined'
	@echo 'Usage: "make PROJECT=<project> PROFILE=<profile> config"'
	@echo '    See templates dir for possible projects and profiles'
	exit 1
endif
	@test -d $(PROJECTS_DIR)/$(PROJECT) \
		|| (echo 'Error: project $(PROJECT) does not exist' \
			&& exit 1)
	@test -d $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE) \
		|| (echo 'Error: profile $(PROFILE) does not exist in project $(PROJECT)' \
			&& exit 1)
	@if [ -d $(CONF_DIR) ];           \
	then                              \
		if [ -d $(BACKUP_DIR) ];      \
		then                          \
			$(RM) -r $(BACKUP_DIR)/*; \
		else                          \
			$(MKDIR) $(BACKUP_DIR);   \
		fi;                           \
		$(if $(filter-out $(BACKUP_DIR),$(wildcard $(CONF_DIR)/*)), \
			mv -fv -t $(BACKUP_DIR) \
				$(filter-out $(BACKUP_DIR),$(wildcard $(CONF_DIR)/*));, \
			rm -r $(BACKUP_DIR); \
		)                             \
	else                              \
		$(MKDIR) $(CONF_DIR);         \
	fi;
	@$(CP) -fv -R -t $(CONF_DIR) \
	  $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)/*);
	@echo 'Config complete'


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

TEMPLATES = $(patsubst $(PROJECTS_DIR)/%,%,$(wildcard $(PROJECTS_DIR)/*/*))

menuconfig m:
	@$(MAKE) TEMPLATE=$$(dialog --stdout --backtitle "Configuration template selection" \
                --menu "Select template to load:" 20 40 20 \
	       	$(foreach t,$(TEMPLATES),$t "" )) config
xconfig x:
	@$(MAKE) TEMPLATE=$$(Xdialog --stdout --backtitle "Configuration template selection" \
                --menu "Select template to load:" 20 40 20 \
	       	$(foreach t,$(TEMPLATES),$t "" )) config

