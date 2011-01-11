#
#   Date: Jan 9, 2011
# Author: Eldar Abusalimov
#

ifndef __util_var_info_mk
__util_var_info_mk := 1

include util/common.mk

##
# Function: var_info_string
#
# Expands to the variable information string.
# Information includes flavor, origin and value of the specified variable.
#
# Params:
#  1. The target variable name
#
# Returns: Human-readable multiline string containing variable information.
#
define var_info_string
$(call assert_called,var_info_string,$0) \
  $(call assert,$1,Variable name is empty) Variable [$1] info:
   flavor: [$(flavor $1)]
   origin: [$(origin $1)]
    value: [$(value $1)]

endef

##
# Function: var_info
#
# Prints the information about the specified variable using $(info) function.
#
# Params:
#  1. The target variable name
#
# Returns: nothing
#
# See also: var_info_string
#
var_info = $(call assert_called,var_info,$0)$(info $(call var_info_string,$1))

var_undefined = $(call make_bool,$(findstring undefined,$(flavor $1)))

endif # __util_var_info_mk
