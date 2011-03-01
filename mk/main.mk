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

ROOT_DIR       := .#

MK_DIR         := $(ROOT_DIR)/mk
SCRIPTS_DIR    := $(ROOT_DIR)/scripts
PROJECTS_DIR   := $(ROOT_DIR)/templates
THIRDPARTY_DIR := $(ROOT_DIR)/third-party
PLATFORM_DIR   := $(ROOT_DIR)/platform
SRC_DIR        := $(ROOT_DIR)/src
DOCS_DIR       := $(ROOT_DIR)/docs

CONF_DIR       := $(ROOT_DIR)/conf

BASE_CONF_DIR  := $(ROOT_DIR)/conf
PATCH_CONF_DIR := $(ROOT_DIR)/conf/$(PATCH_NAME)

BACKUP_DIR     := $(ROOT_DIR)/conf/backup~

BUILD_DIR     := $(ROOT_DIR)/build/$(if $(PATCH_NAME),patch_$(PATCH_NAME),base)

EMBUILD_DIR    := $(MK_DIR)/embuild

BIN_DIR        := $(BUILD_DIR)/bin
OBJ_DIR        := $(BUILD_DIR)/obj
LIB_DIR        := $(BUILD_DIR)/lib
DOT_DIR        := $(BUILD_DIR)/dot
DOCS_OUT_DIR   := $(BUILD_DIR)/docs
CODEGEN_DIR    := $(BUILD_DIR)/codegen
AUTOCONF_DIR   := $(CODEGEN_DIR)
ROMFS_DIR      := $(ROOT_DIR)/romfs

RM     := rm -f
CP     := cp
PRINTF := printf
SHELL  := bash
MKDIR  := mkdir -p
LN     := ln -s

TEMPLATES = $(notdir $(wildcard $(PROJECTS_DIR)/*))

include gmd.mk

include rules.mk
include util.mk

include gmsl/gmsl.mk

makegoals := $(MAKECMDGOALS)
ifeq ($(makegoals),)
makegoals := all
endif

# XXX Fix this shit. -- Eldar

# 'clean', 'docsgen' and 'config' are handled in-place.
ifneq ($(filter-out %clean %config %docsgen,$(makegoals)),)
# Root 'make all' does not need other makefiles too.
ifneq ($(or $(filter-out $(makegoals),all),$(BUILD_TARGET)),)
# Need to include it prior to walking the source tree
include $(MK_DIR)/configure.mk
# Skip image.mk if configs has not been remade yet
ifneq ($(wildcard $(AUTOCONF_DIR)/build.mk),)
include $(MK_DIR)/image.mk
include $(MK_DIR)/codegen-dot.mk
endif # $(wildcard $(AUTOCONF_DIR)/build.mk)
endif # $(or $(filter-out $(makegoals),all),$(BUILD_TARGET))
endif # $(filter-out %clean %config %docsgen,$(makegoals))

__get_subdirs = $(sort $(notdir $(call d-wildcard,$(1:%=%/*))))
build_patch_targets := \
  $(patsubst %,%.target, \
    $(filter-out $(notdir $(BACKUP_DIR)),$(call __get_subdirs, $(CONF_DIR))) \
  )

.PHONY: all build prepare docsgen dot clean config xconfig menuconfig
.PHONY: $(build_patch_targets) build_base_target romfs create_romfs

all: $(build_patch_targets) build_base_target
	@echo 'Build complete'

$(build_patch_targets): export PATCH_NAME=$(basename $@)
$(build_patch_targets) build_base_target: export BUILD_TARGET=1
$(build_patch_targets) build_base_target:
	$(MAKE) build

build: check_config prepare image
	@echo '$(or $(PATCH_NAME),Base) build complete'

prepare:
	@$(MKDIR) $(BUILD_DIR)
	@$(MKDIR) $(BIN_DIR)
	@$(MKDIR) $(OBJ_DIR)
	@$(MKDIR) $(LIB_DIR)
	@$(MKDIR) $(AUTOCONF_DIR)
	@$(MKDIR) $(DOCS_OUT_DIR)

docsgen:
	@[ -d $(DOCS_OUT_DIR) ] || $(MKDIR) $(DOCS_OUT_DIR)
	doxygen
	#@$(MAKE) -C $(DOCS_DIR)
	@echo 'Docs generation complete'

dot: $(GRAPH_PS)
	@echo 'Dot complete'

create_romfs: build
	@$(RM) -rv $(ROMFS_DIR)
	@$(MKDIR) $(ROMFS_DIR)
	$(CP) $(BUILD_DIR)/bin/embox $(ROMFS_DIR)
	pushd $(ROMFS_DIR); find ./ -depth -print | cpio -H newc -ov > ../ramfs.cpio; popd;

clean c: _clean
	@echo 'Clean complete'

distclean dc: _distclean
	@echo 'Distclean complete'

_clean:
	@$(RM) -rv $(ROOT_DIR)/build

_distclean: _clean
	@$(RM) -rv $(BACKUP_DIR)
	@$(RM) -rv $(CONF_DIR)


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
ifeq (0,1)
# That's an old variant of config creating system.
# It will be removed soon.
# Just not to search long if smth goes wrong %)
	@$(foreach dir,$(call __get_subdirs,$(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)), \
	  $(MKDIR) $(CONF_DIR)/$(dir); \
	  $(CP) -fv -t $(CONF_DIR)/$(dir) \
	     $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)/*); \
	  $(if $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)/$(dir)/*), \
	    $(CP) -fv -t $(CONF_DIR)/$(dir) \
	       $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)/$(dir)/*); \
	  ) \
	)
else
# That's a new variant.
ifdef SYMLINK_CONF_DIR
# We just symlink template dir to conf dir
	@$(RM) -r $(CONF_DIR)
	@$(LN) $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE) $(CONF_DIR)
else
# We just copy from templates dir to conf dir
	@$(CP) -fv -R -t $(CONF_DIR) \
	  $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)/*);
endif
endif
	@echo 'Config complete'

CUR_CONFIG_FILES := $(filter-out $(notdir $(BACKUP_DIR)),\
						$(notdir $(wildcard $(BASE_CONF_DIR)/*)))
# It would be better to use check_config from configure.mk,
# But I cant imagine any normal condition to include it.
saveconfig s:
ifndef PROJECT
	@echo 'Error: PROJECT undefined'
	@echo 'Usage: "make PROJECT=<project> PROFILE=<new profile name> saveconfig"'
	exit 1
endif
ifndef PROFILE
	@echo 'Error: PROFILE undefined'
	@echo 'Usage: "make PROJECT=<project> PROFILE=<new profile name> saveconfig"'
	exit 1
endif
#	@if [-d $(BASE_CONF_DIR)];                                           \
	then                                                                 \
		echo 'Error: conf - folder not found. No config to be saved.';   \
		exit 1; \
	fi;
	$(if $(CUR_CONFIG_FILES),,\
		echo 'Error: no config presented in "$(BASE_CONF_DIR)"'; \
		exit 1; \
	)
ifneq ($(FORCED),true)
	@if [ -d $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE) ];          \
	then                                                       \
		echo 'Error: Profile "$(PROFILE)" already exist';      \
		exit 1;                                                \
	fi;
else
	rm -r $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE);
endif
	$(MKDIR) $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE);        \
	$(CP) -fvr -t $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)/ \
			$(CUR_CONFIG_FILES:%=$(BASE_CONF_DIR)/%);
	@echo Config was saved.

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
	@$(EDIT) $(CONF_DIR)/*.conf
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
	@$(EDIT) $(CONF_DIR)/*.conf
	@$(RM) .tmp

