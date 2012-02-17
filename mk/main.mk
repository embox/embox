#
# Embox main Makefile
#

# Check Make version (we need at least GNU Make 3.81)
# .FEATURES built-in variable has been introduced exactly in GNU Make 3.81.
ifneq ($(origin .FEATURES),default)
define error_msg
Unsupported Make version.
Unfortunatelly EMBuild does not work properly with GNU Make $(MAKE_VERSION).
This is a known issue. Please use GNU Make 3.81 or above
endef
endif

# Root makefile runs this using single goal per invocation. Check it.
ifneq ($(words $(MAKECMDGOALS)),1)
define error_msg
Illegal invocation (non-single make goal).
Please use root makefile to run the build
endef
endif

# Fail here if things are bad...
ifdef error_msg
$(error $(error_msg))
endif

include mk/dirs.mk

MY_PATH := $(SRC_DIR)/** $(PLATFORM_DIR)/** $(THIRDPARTY_DIR)/**

RM     := rm -f
CP     := cp
PRINTF := printf
SHELL  := bash
MKDIR  := mkdir -p
LN     := ln -s
PS1    :=

include mk/util.mk
include mk/util/wildcard.mk
include mk/util/serialize.mk

include mk/gmsl/gmsl.mk

# XXX Fix this shit. -- Eldar

# 'clean', 'docsgen' and 'config' are handled in-place.
ifneq ($(filter-out %clean %config conf% %docsgen,$(MAKECMDGOALS)),)
# Need to include it prior to walking the source tree
include $(MK_DIR)/configure.mk
# Skip image.mk if configs has not been remade yet
ifneq ($(wildcard $(AUTOCONF_DIR)/build.mk),)
include $(MK_DIR)/image.mk
include $(MK_DIR)/codegen-dot.mk
endif # $(wildcard $(AUTOCONF_DIR)/build.mk)
endif # $(filter-out %clean %config %docsgen,$(MAKECMDGOALS))

.PHONY: all  prepare docsgen dot clean config xconfig menuconfig conf_update

all: check_config prepare image
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

TEMPLATES = $(notdir $(wildcard $(PROJECTS_DIR)/*))

menuconfig m: PROJECT = $(shell dialog \
                --stdout --backtitle "Configuration template selection" \
                --radiolist "Select project to load:" 10 40 \
                $(shell echo $(TEMPLATES) | wc -w) \
                $(patsubst %,% "" off,$(TEMPLATES)) | tee .tmp)
menuconfig m: PROFILE = $(shell dialog \
                --stdout --backtitle "Configuration template selection" \
                --radiolist "Select profile to load:" 10 40 \
                $(shell echo $(notdir $(wildcard $(PROJECTS_DIR)/$(shell cat .tmp)/*)) | wc -w) \
                $(patsubst %,% "" off,$(notdir $(wildcard $(PROJECTS_DIR)/$(shell cat .tmp)/*))))
menuconfig m: EDIT = $(shell dialog \
                --stdout --backtitle "Editor selection" \
                --radiolist "Select editor:" 20 40 2 "emacs -nw -Q" "" on vim "" off)
menuconfig m:
	@$(MAKE) PROJECT=$(PROJECT) PROFILE=$(PROFILE) config
	@$(RM) .tmp

xconfig x: PROJECT = $(shell Xdialog \
                --stdout --backtitle "Configuration template selection" \
                --radiolist "Select project to load:" 20 40 \
                $(shell echo $(TEMPLATES) | wc -w) \
                $(patsubst %,% "" off,$(TEMPLATES)) | tee .tmp)
xconfig x: PROFILE = $(shell Xdialog \
                --stdout --backtitle "Configuration template selection" \
                --radiolist "Select profile to load:" 20 40 \
                $(shell echo $(notdir $(wildcard $(PROJECTS_DIR)/$(shell cat .tmp)/*)) | wc -w) \
                $(patsubst %,% "" off,$(notdir $(wildcard $(PROJECTS_DIR)/$(shell cat .tmp)/*))))
xconfig x: EDIT = $(shell Xdialog \
                --stdout --backtitle "Editor selection" \
                --radiolist "Select editor:" 20 40 2 emacs "" on gvim "" off)
xconfig x:
	@$(MAKE) PROFILE=$(PROFILE) PROJECT=$(PROJECT) config
	@$(RM) .tmp

