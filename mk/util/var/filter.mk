#
# Provides 'filter' and 'filter-out' facilities for lists of arbitrary named
# variables (e.g. with whitespaces in their names).
#
#   Date: Nov 19, 2010
# Author: Eldar Abusalimov
#

ifndef __util_var_filter_mk
__util_var_filter_mk := 1

include util/var/name.mk
include util/list.mk

##
# Function: var_list_map
# Calls the specified function on each defined variable from the specified
# list.
#
# Params:
#  1. Name of function to call on each defined variable,
#     with the following signature:
#       1. Variable name
#       2. Optional argument (if any)
#      Return: the value to append to the resulting list
#  2. (optional) List of variables to iterate over
#     If not specified, the value of .VARIABLES built-in variable is used
#  3. Optional argument to pass when calling the function
# Return: The result of calling the function on each variable
#
var_list_map = \
 $(call list_map_transcoded,$1, \
   $(or $(value 2),$(.VARIABLES)),var_name_mangle,var_name_demangle,$(value 3))

##
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
#  4. Optional argument to pass when calling the function
#
# Return: result of calling the specified function on each filtered variable
#
var_filter = $(call __var_filter,filter,$1,$2,$3,$(value 4))

##
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
#  4. Optional argument to pass when calling the function
#
# Return: result of calling the specified function on each filtered variable
#
var_filter_out = $(call __var_filter,filter-out,$1,$2,$3,$(value 4))

# The real work is done here
# Params:
#  1. Filtering function (filter/filter-out)
#  2. Variables to check for the match with
#  3. Variables list being filtered
#  4. Function to call for each filtered variable
#  5. Argument to pass when calling the function
# Return: result of calling the specified function on each filtered variable
__var_filter = $(call list_map_transcoded,$4, \
  $(call $1,$(call var_name_mangle,$2), \
            $(call var_name_mangle,$3)),,var_name_demangle,$5)

endif # __util_var_filter_mk
