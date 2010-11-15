#
# Author: Eldar Abusalimov
#

include util.mk
include util/wildcard.mk
include util/var.mk

traverse = $(call r-wildcard,$(1:%=%/**/*.em))

export SRC_DIR = mk/test
export OBJ_DIR = build/obj
export PRINTF := printf

em_files = $(call traverse,$(SRC_DIR))
entity_files = $(em_files:$(SRC_DIR)/%.em=$(OBJ_DIR)/%.mk)

-include $(entity_files)

$(entity_files) : $(OBJ_DIR)/%.mk : $(SRC_DIR)/%.em
	$(MAKE) -f mk/embuild/traverse/emfile.mk all \
		__EMBUILD_TRAVERSE_EMFILE_ROOT=$(SRC_DIR) \
		__EMBUILD_TRAVERSE_EMFILE=$*.em \

all:
clean:
	rm -rf build
