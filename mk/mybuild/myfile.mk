#
# MyFile facade.
#
#   Date: Dec 20, 2011
# Author: Eldar Abusalimov
#

ifndef __mybuild_myfile_mk
__mybuild_myfile_mk := $(lastword $(MAKEFILE_LIST))

include $(addprefix $(basename $(__mybuild_myfile_mk)), \
			-model-old.mk -parser.mk)

$(def_all)

endif # __mybuild_myfile_mk

