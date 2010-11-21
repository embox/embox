#
# Provides 'filter' and 'filter-out' facilities for lists of arbitrary named
# variables (e.g. with whitespaces in their names).
#
#   Date: Nov 19, 2010
# Author: Eldar Abusalimov
#

ifndef __util_var_filter_mk
__util_var_filter_mk := 1

include util/var_name.mk

#
# Function: var_filter
#
# Filters variables listed in the first argument from the list in the second
# calling the specified function for each filtered one.
#
# Params:
#  1. Variables to check for the match with
#  2. Variables list being filtered
#  3. Function to call for each occurrence of variable from the first list
#     in the second
#
# Returns: result of calling the specified function on each filtered variable
#
var_filter = $(call __var_filter,filter,$1,$2,$3)

#
# Function: var_filter_out
#
# Filters out variables listed in the first argument from the list in the
# second calling the specified function for each leftover one.
#
# Params:
#  1. Variables to check for the match with
#  2. Variables list being filtered
#  3. Function to call for each variable of the second list which is not
#     presented in the first one
#
# Returns: result of calling the specified function on each filtered variable
#
var_filter_out = $(call __var_filter,filter-out,$1,$2,$3)

__var_filter = $(foreach __var, \
         $(call $1,$(call var_name_escape,$2),$(call var_name_escape,$3)) \
                 ,$(call $4,$(call var_name_unescape,$(__var))))

endif # __util_var_filter_mk
