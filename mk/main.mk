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
TEMPLATES_DIR  := $(ROOT_DIR)/templates
THIRDPARTY_DIR := $(ROOT_DIR)/third-party
PLATFORM_DIR   := $(ROOT_DIR)/platform
SRC_DIR        := $(ROOT_DIR)/src

CONF_DIR       := $(ROOT_DIR)/conf
BACKUP_DIR     := $(ROOT_DIR)/conf/backup~

BUILD_DIR      := $(ROOT_DIR)/build
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

makegoals:=$(MAKECMDGOALS)
ifeq ($(makegoals),)
makegoals:=all
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

.PHONY: all prepare docs dot clean config xconfig menuconfig

all: check_config prepare image
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
	@echo 'Usage: "make TEMPLATE=<profile> config"'
	@echo '    See templates dir for possible profiles'
	exit 1
endif
	@test -d $(TEMPLATES_DIR)/$(TEMPLATE) \
		|| (echo 'Error: template $(TEMPLATE) does not exist' \
		&& exit 1)
	@if [ -d $(CONF_DIR) ];           \
	then                              \
		if [ -d $(BACKUP_DIR) ];      \
		then                          \
			$(RM) -r $(BACKUP_DIR)/*; \
		else                          \
			mkdir -p $(BACKUP_DIR);   \
		fi;                           \
		mv -fv -t $(BACKUP_DIR)       \
			$(filter-out $(BACKUP_DIR),$(wildcard $(CONF_DIR)/*)); \
	else                              \
		mkdir -p $(CONF_DIR);         \
	fi;
	@cp -fv -t $(CONF_DIR) \
		$(wildcard $(TEMPLATES_DIR)/$(TEMPLATE)/*)
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
