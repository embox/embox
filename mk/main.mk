#
# EMBOX main Makefile
#

# The first target refers to "all"
.PHONY: default
default: all

ifndef ROOT_DIR
$(error ROOT_DIR undefined)
endif

MK_DIR       :=$(ROOT_DIR)/mk
CONF_DIR     :=$(ROOT_DIR)/conf
SCRIPTS_DIR  :=$(ROOT_DIR)/scripts
TEMPLATES_DIR:=$(ROOT_DIR)/templates
SRC_DIR      :=$(ROOT_DIR)/src

BUILD_DIR    :=$(ROOT_DIR)/build
BIN_DIR      :=$(BUILD_DIR)/bin
OBJ_DIR      :=$(BUILD_DIR)/obj
DOCS_DIR     :=$(BUILD_DIR)/docs
BUILDCONF_DIR:=$(BUILD_DIR)/conf

LDSCRIPT =$(OBJ_DIR)/arch/$(ARCH)/embox.lds

RM          :=rm -f
EDITOR      :=vim

# Need to include it prior to walking the source tree
# (particularly because of ARCH definition).
include $(MK_DIR)/configure.mk
include $(MK_DIR)/traverse.mk

# Clear some variables, switch them to immediate expansion mode.
OBJS_ALL:=
DIRS_ALL:=

# This code is executed each time when per-directory makefile is processed.
define TRAVERSE_CALLBACK
  obj_node_dir:=$(NODE_DIR:$(SRC_DIR)/%=$(OBJ_DIR)/%)
  OBJS_ALL+=$$(addprefix $$(obj_node_dir)/,$(NODE_OBJS))
  DIRS_ALL+=$$(obj_node_dir)
endef

# Walk the directory tree starting at $(SRC_DIR)
# and searching for Makefile in each sub-directory.
$(eval $(call TRAVERSE,$(SRC_DIR),Makefile,TRAVERSE_CALLBACK))

# Process dependency files.
-include $(OBJS_ALL:.o=.d)

# image.mk should be included after OBJS_ALL is filled in.
include $(MK_DIR)/image.mk
include $(MK_DIR)/rules.mk

.PHONY: all check_config prepare image

all: check_config prepare image

prepare:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BUILDCONF_DIR)
	@mkdir -p $(DIRS_ALL)

.PHONY: docs clean config xconfig menuconfig mconfig

docs:
	doxygen

clean:
	@$(RM) -r $(BUILD_DIR)
#	@$(RM) -r $(BIN_DIR) $(OBJ_DIR) $(DOCS_DIR) $(BUILDCONF_DIR)
#	@$(RM) $(OBJS_ALL)
#	@$(RM) $(OBJS_ALL:.o=.d)
#	@$(RM) .config.old
#	@$(SCRIPTS_DIR)/ConfigBuilder/Misc/checksum.py \
#		-o $(OD_TOOL) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD) --clean
	@echo 'Clean complete'

config:
ifndef TEMPLATE
	@echo 'Error: TEMPLATE undefined'
	@echo 'Usage: "make TEMPLATE=<profile> [OVERWRITE=1] config"'
	@echo '    See templates/ folder for possible profiles'
	exit 1
endif
	@test -d $(TEMPLATES_DIR)/$(TEMPLATE) \
		|| (echo 'Error: template $(TEMPLATE) does not exist' \
		&& exit 1)
ifeq ($(OVERWRITE),1)
	@test -d $(CONF_DIR) \
		&& rm -rf $(CONF_DIR)/* \
		|| mkdir -p $(CONF_DIR)
	@cp -t $(CONF_DIR) $(addprefix $(TEMPLATES_DIR)/$(TEMPLATE)/,$(CONF_FILES))
else
	@test -d $(CONF_DIR) \
		|| mkdir -p $(CONF_DIR)
	@cp -u -t $(CONF_DIR) $(addprefix $(TEMPLATES_DIR)/$(TEMPLATE)/,$(CONF_FILES))
endif

menuconfig:
	@$(EDITOR) $(CONF_DIR)/config.mk
#	@$(SCRIPTS_DIR)/configure.py --mode=menu > /dev/null 2>&1

xconfig:
#	@$(SCRIPTS_DIR)/configure.py --mode=tk > /dev/null 2>&1

mconfig:
#	@$(SCRIPTS_DIR)/configure.py --mode=qt > /dev/null 2>&1

