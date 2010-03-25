#
# EMBOX main Makefile
#

# The first target is "all"
all:

ifeq ($(MAKE_VERSION),3.80)
define ERROR
Unsupported GNU Make version.
Unfortunatelly EMBuild does not work properly with GNU Make $(MAKE_VERSION)
This is a known issue. Please use GNU Make version 3.81
endef
$(error $(ERROR))
endif

ifndef ROOT_DIR
$(error ROOT_DIR undefined)
endif

MK_DIR         := $(ROOT_DIR)/mk
SCRIPTS_DIR    := $(ROOT_DIR)/scripts
PROJECTS_DIR  := $(ROOT_DIR)/templates
THIRDPARTY_DIR := $(ROOT_DIR)/third-party
PLATFORM_DIR   := $(ROOT_DIR)/platform
SRC_DIR        := $(ROOT_DIR)/src

ifndef PATCH_NAME
#	the only case it's not defined - when target is config
#	In that case CONF_DIR must refer to ./conf
CONF_DIR       := $(ROOT_DIR)/conf
else
CONF_DIR       := $(ROOT_DIR)/conf/$(PATCH_NAME)
endif

BACKUP_DIR     := $(ROOT_DIR)/conf/backup~

BUILD_DIR      := $(ROOT_DIR)/build/$(PATCH_NAME)
BIN_DIR        := $(BUILD_DIR)/bin
OBJ_DIR        := $(BUILD_DIR)/obj
LIB_DIR        := $(OBJ_DIR)
DOT_DIR        := $(BUILD_DIR)/dot
DOCS_DIR       := $(BUILD_DIR)/docs
CODEGEN_DIR    := $(BUILD_DIR)/codegen
AUTOCONF_DIR   := $(CODEGEN_DIR)

RM     := rm -f
CP     := cp
EDIT   := emacs
PRINTF := printf

TEMPLATES = $(notdir $(wildcard $(TEMPLATES_DIR)/*))

include $(MK_DIR)/util.mk

makegoals := $(MAKECMDGOALS)
ifeq ($(makegoals),)
makegoals := all
endif

ifdef PATCH_NAME
# Need to include it prior to walking the source tree
# (particularly because of ARCH definition).
include $(MK_DIR)/configure.mk
endif

ifneq ($(wildcard $(AUTOCONF_DIR)/build.mk),)
include $(MK_DIR)/image.mk
include $(MK_DIR)/codegen-dot.mk
endif

__get_subdirs = $(sort $(notdir $(call d-wildcard,$(1:%=%/*))))
build_targets := $(patsubst %,%.target,$(filter-out $(notdir $(BACKUP_DIR)), \
  $(call __get_subdirs, $(CONF_DIR))))

.PHONY: all build prepare docs dot clean config xconfig menuconfig
.PHONY: $(build_targets)

all: $(build_targets)
	@echo 'Build complete'

$(build_targets): export PATCH_NAME = $(basename $@)
$(build_targets):
	$(MAKE) build

build: check_config prepare image
	@echo '$(PATCH_NAME) build complete'

prepare:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(AUTOCONF_DIR)

docs:
	@mkdir -p $(DOCS_DIR) && doxygen

dot: $(GRAPH_PS)
	@echo 'Dot complete'

clean: _clean
	@echo 'Clean complete'

distclean: _distclean
	@echo 'Distclean complete'

_clean:
	@$(RM) -rv $(BUILD_DIR)
#	@$(RM) -r $(BIN_DIR) $(OBJ_DIR) $(DOCS_DIR) $(AUTOCONF_DIR)
#	@$(RM) $(OBJS_ALL)
#	@$(RM) $(OBJS_ALL:.o=.d)
#	@$(RM) .config.old
#	@$(SCRIPTS_DIR)/ConfigBuilder/Misc/checksum.py \
#		-o $(OD_TOOL) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD) --clean

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
			mkdir -p $(BACKUP_DIR);   \
		fi;                           \
		$(if $(filter-out $(BACKUP_DIR),$(wildcard $(CONF_DIR)/*)), \
			mv -fv -t $(BACKUP_DIR) \
				$(filter-out $(BACKUP_DIR),$(wildcard $(CONF_DIR)/*));, \
			rm -r $(BACKUP_DIR); \
		) \
	fi;
	@$(foreach dir,$(call __get_subdirs,$(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)), \
	  mkdir -p $(CONF_DIR)/$(dir); \
	  cp -fv -t $(CONF_DIR)/$(dir) \
	     $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)/*); \
	  $(if $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)/$(dir)/*), \
	    cp -fv -t $(CONF_DIR)/$(dir) \
	       $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(PROFILE)/$(dir)/*); \
	  ) \
	)
	@echo 'Config complete'

menuconfig:
	make PROFILE=`dialog \
		--stdout --backtitle "Configuration template selection" \
		--radiolist "Select template to load:" 10 40 3 \
		$(patsubst %,% "" off,$(TEMPLATES))` \
	config
	@$(EDIT) -nw $(CONF_DIR)/*.conf

xconfig:
	make PROFILE=`Xdialog \
		--stdout --backtitle "Configuration template selection" \
		--radiolist "Select template to load:" 20 40 3 \
		$(patsubst %,% "" off,$(TEMPLATES))` \
	config
	@$(EDIT) $(CONF_DIR)/*.conf
