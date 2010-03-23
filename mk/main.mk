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

ifndef PROJECT_NAME
#	the only case it's not defined - when target is config
#	In that case CONF_DIR must refer to ./conf
CONF_DIR       := $(ROOT_DIR)/conf
else
CONF_DIR       := $(ROOT_DIR)/conf/$(PROJECT_NAME)
endif

BACKUP_DIR     := $(ROOT_DIR)/conf/backup~

BUILD_DIR      := $(ROOT_DIR)/build/$(PROJECT_NAME)
BIN_DIR        := $(BUILD_DIR)/bin
OBJ_DIR        := $(BUILD_DIR)/obj
LIB_DIR        := $(OBJ_DIR)
DOT_DIR        := $(BUILD_DIR)/dot
DOCS_DIR       := $(BUILD_DIR)/docs
CODEGEN_DIR    := $(BUILD_DIR)/codegen
AUTOCONF_DIR   := $(CODEGEN_DIR)

CUR_TEMPLATE_FILES := $(wildcard $(TEMPLATES_DIR)/$(TEMPLATE)/*)

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
ifeq ($(filter %clean %config,$(makegoals)),)
# Need to include it prior to walking the source tree
# (particularly because of ARCH definition).
include $(MK_DIR)/configure.mk
ifneq ($(wildcard $(AUTOCONF_DIR)/build.mk),)
include $(MK_DIR)/image.mk
include $(MK_DIR)/codegen-dot.mk
endif
endif

__get_subdirs = $(sort $(notdir $(call d-wildcard,$(1:%=%/*))))

.PHONY: all
all:
	$(foreach target_name, $(call __get_subdirs, $(CONF_DIR)), \
	$(MAKE) -C $(ROOT_DIR)/ PROJECT_NAME=$(target_name) build_target;     \
	)

.PHONY: build_target prepare docs dot clean config xconfig menuconfig

#	ex- all
build_target: check_config prepare image
	@echo 'Build complete'

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
ifndef TEMPLATE
	@echo 'Error: TEMPLATE undefined'
	@echo 'Usage: "make PROJECT=<proj_name> TEMPLATE=<profile> config"'
	@echo '    See templates dir for possible projects and profiles'
	exit 1
endif
ifndef PROJECT
	@echo 'Error: PROJECT undefined'
	@echo 'Usage: "make PROJECT=<proj_name> TEMPLATE=<profile> config"'
	@echo '    See templates dir for possible projects and profiles'
	exit 1
endif
	@test -d $(PROJECTS_DIR)/$(PROJECT) \
		|| (echo 'Error: project $(PROJECT) does not exist' \
		&& exit 1)
	@test -d $(PROJECTS_DIR)/$(PROJECT)/$(TEMPLATE) \
		|| (echo 'Error: template $(TEMPLATE) does not exist in project $(PROJECT)' \
		&& exit 1)
	@if [ -d $(CONF_DIR) ];           \
	then                              \
		if [ -d $(BACKUP_DIR) ];      \
		then                          \
			$(RM) -r $(BACKUP_DIR)/*; \
		else                          \
			mkdir -p $(BACKUP_DIR);   \
		fi;                           \
		mv -fv -t $(BACKUP_DIR) \
			$(filter-out $(BACKUP_DIR),$(wildcard $(CONF_DIR)/*)); \
	fi;
	$(foreach dir, $(call __get_subdirs, $(PROJECTS_DIR)/$(PROJECT)/$(TEMPLATE)), \
	  mkdir -p $(CONF_DIR)/$(dir); \
	  cp -fv -t $(CONF_DIR)/$(dir) \
	     $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(TEMPLATE)/*); \
	if [-d $(PROJECTS_DIR)/$(PROJECT)/$(TEMPLATE)/$(dir)/*]; \
	then \
	  cp -fv -t $(CONF_DIR)/$(dir) \
	     $(wildcard $(PROJECTS_DIR)/$(PROJECT)/$(TEMPLATE)/$(dir)/*); \
	fi; \
	)
	@echo 'Config complete'

menuconfig:
	make TEMPLATE=`dialog \
		--stdout --backtitle "Configuration template selection" \
		--radiolist "Select template to load:" 10 40 3 \
		$(patsubst %,% "" off,$(TEMPLATES))` \
	config
	@$(EDIT) -nw $(CONF_DIR)/*.conf

xconfig:
	make TEMPLATE=`Xdialog \
		--stdout --backtitle "Configuration template selection" \
		--radiolist "Select template to load:" 20 40 3 \
		$(patsubst %,% "" off,$(TEMPLATES))` \
	config
	@$(EDIT) $(CONF_DIR)/*.conf
