#
# EMBOX main Makefile
#

ifndef ROOT_DIR
$(error ROOT_DIR undefined)
endif

MK_DIR      :=$(ROOT_DIR)/mk
CONF_DIR    :=$(ROOT_DIR)/templates/sparc
SCRIPTS_DIR :=$(ROOT_DIR)/scripts

SRC_DIR     :=$(ROOT_DIR)/src
DOCS_DIR    :=$(ROOT_DIR)/docs

BUILD_DIR   :=$(ROOT_DIR)/bin
BIN_DIR     :=$(ROOT_DIR)/bin
OBJ_DIR     :=$(ROOT_DIR)/obj

RM          :=rm -f
EDITOR      :=vim

.PHONY: all check_config image
all: check_config

include $(MK_DIR)/rules.mk
include $(MK_DIR)/configure.mk
include $(MK_DIR)/traverse.mk

TRAVERSE_CALLBACK = \
    OBJS_ALL :=$$(OBJS_ALL) $$(addprefix $(NODE_DIR)/,$(NODE_OBJS))
$(eval $(call TRAVERSE,$(SRC_DIR),Makefile,TRAVERSE_CALLBACK))
-include $(OBJS_ALL:.o=.d)

include $(MK_DIR)/image.mk

all: image

.PHONY: docs clean config xconfig menuconfig mconfig

docs:
	doxygen

clean:
	@$(RM) .config.old
	@$(RM) -r $(BIN_DIR) $(BUILD_DIR) $(OBJ_DIR) $(DOCS_DIR)
	@$(RM) $(OBJS_ALL)
	@$(RM) $(OBJS_ALL:.o=.d)
#	@$(SCRIPTS_DIR)/ConfigBuilder/Misc/checksum.py \
#		-o $(OD_TOOL) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD) --clean
	@echo 'Clean complete'

config:
	@cp $(CONF_DIR)/config.in $(CONF_DIR)/config.mk

menuconfig:
	@$(EDITOR) $(CONF_DIR)/config.mk
#	@$(SCRIPTS_DIR)/configure.py --mode=menu > /dev/null 2>&1

xconfig:
#	@$(SCRIPTS_DIR)/configure.py --mode=tk > /dev/null 2>&1

mconfig:
#	@$(SCRIPTS_DIR)/configure.py --mode=qt > /dev/null 2>&1

