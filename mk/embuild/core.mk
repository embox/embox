#
# Author: Eldar Abusalimov
#

include util.mk
include util/file.mk
include util/var.mk

traverse = $(call r-wildcard,$(1:%=%/**/*.em))

export SRC_DIR = mk/test
export OBJ_DIR = build/obj
export PRINTF := printf

em_files = $(call traverse,$(SRC_DIR))
entity_files = $(em_files:$(SRC_DIR)/%.em=$(OBJ_DIR)/%.mk)

#$(error $(em_files))

include $(entity_files)

$(entity_files) : $(OBJ_DIR)/%.mk : $(SRC_DIR)/%.em
	$(MAKE) -f embuild/traverse/sandbox.mk EMBUILD_ENTITY=$* $@

all:
