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
LIB_DIR      :=$(BUILD_DIR)/lib
DOCS_DIR     :=$(BUILD_DIR)/docs
BUILDCONF_DIR:=$(BUILD_DIR)/conf

RM          :=rm -f
EDITOR      :=emacs

# Need to include it prior to walking the source tree
# (particularly because of ARCH definition).
include $(MK_DIR)/configure.mk
include $(MK_DIR)/rules.mk

include $(MK_DIR)/image.mk

.PHONY: all prepare docs clean config xconfig menuconfig mconfig

all: check_config prepare image
	@echo 'Build complete'

prepare:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(BUILDCONF_DIR)
	@mkdir -p $(DIRS_ALL)

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
	@echo 'Config complete'

menuconfig:
	make TEMPLATE=sparc config
	@$(EDITOR) -nw $(CONF_DIR)/*.conf

xconfig:
	make TEMPLATE=sparc config
	@$(EDITOR) $(CONF_DIR)/*.conf
