#
#   Date: Jan 9, 2011
# Author: Eldar Abusalimov
#

ifndef __util_var_info_mk
__util_var_info_mk := 1

include core/common.mk

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
$(call assert,$1,Variable name is empty)Variable [$1] info:
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
var_info = \
  $(info $(call var_info_string,$1))

##
# Function: var_defined
#
# Checks whether the specified variable is defined.
#
# Params:
#  1. The target variable name
#
# Returns: true if the specified variable exists, false otherwise
#
var_defined = \
  $(call not,$(call var_undefined,$1))

##
# Function: var_simple
#
# Checks whether the specified variable is defined as statically expanded.
#
# Params:
#  1. The target variable name
#
# Returns: true if the specified variable exists and its flavor is 'simple',
#          false otherwise
#
var_simple = \
  $(call make_bool,$(findstring simple,$(flavor $1)))

##
# Function: var_recursive
#
# Checks whether the specified variable is defined as recursively expanded.
#
# Params:
#  1. The target variable name
#
# Returns: true if the specified variable exists and its flavor is 'recursive',
#          false otherwise
#
var_recursive = \
  $(call make_bool,$(findstring recursive,$(flavor $1)))

##
# Function: var_undefined
#
# Checks whether the specified variable is not defined.
#
# Params:
#  1. The target variable name
#
# Returns: true if the variable has not been defined yet, false otherwise
#
var_undefined = \
  $(call make_bool,$(findstring undefined,$(flavor $1)))

endif # __util_var_info_mk
