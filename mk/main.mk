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
AUTOCONF_DIR :=$(BUILD_DIR)/conf

RM          :=rm -f
EDITOR      :=emacs

makegoals:=$(MAKECMDGOALS)
ifeq ($(makegoals),)
makegoals:=all
endif
ifneq ($(filter all,$(makegoals)),)
# Need to include it prior to walking the source tree
# (particularly because of ARCH definition).
include $(MK_DIR)/configure.mk
include $(MK_DIR)/image.mk
endif

.PHONY: all prepare docs clean config xconfig menuconfig mconfig

all: check_config prepare image
	@echo 'Build complete'

prepare:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(AUTOCONF_DIR)
	@mkdir -p $(DIRS_ALL)

docs:
	doxygen

clean: _clean
	@echo 'Clean complete'

distclean: _distclean
	@echo 'Distclean complete'

_clean:
	$(RM) -r $(BUILD_DIR)
#	@$(RM) -r $(BIN_DIR) $(OBJ_DIR) $(DOCS_DIR) $(AUTOCONF_DIR)
#	@$(RM) $(OBJS_ALL)
#	@$(RM) $(OBJS_ALL:.o=.d)
#	@$(RM) .config.old
#	@$(SCRIPTS_DIR)/ConfigBuilder/Misc/checksum.py \
#		-o $(OD_TOOL) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD) --clean

_distclean: _clean
	$(RM) -r $(CONF_DIR)

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
	@test -d $(CONF_DIR) \
		|| mkdir -p $(CONF_DIR)
	cp -f -v -b -t $(CONF_DIR) $(TEMPLATES_DIR)/$(TEMPLATE)/*
	@echo 'Config complete'

menuconfig:
	make TEMPLATE=sparc config
	@$(EDITOR) -nw $(CONF_DIR)/*.conf

xconfig:
	make TEMPLATE=sparc config
	@$(EDITOR) $(CONF_DIR)/*.conf
