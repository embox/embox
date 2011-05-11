#
# Provides 'filter' and 'filter-out' facilities for lists of arbitrary named
# variables (e.g. with whitespaces in their names).
#
#   Date: Nov 19, 2010
# Author: Eldar Abusalimov
#

ifndef __util_var_list_mk
__util_var_list_mk := 1

include util/var/name.mk
include util/list.mk

##
# Handler function interface for variable list mapping/filtering functions.
#
# Params:
#  1. Variable name
#  2. Optional argument (if any)
# Return:
#     The value to append to the resulting list.
var_list_map_fn = $1

##
# Handler method interface for variable list mapping/filtering functions.
#
# Params:
#  1. This
#  2. Variable name
#  3. Optional argument (if any)
# Return:
#     The value to append to the resulting list.
var_list_map_invoke_fn = $2

##
# Calls the specified function on each defined variable from the specified
# list.
#
# Params:
#  1. Name of function to call on each defined variable, see 'var_list_map_fn'
#  2. (optional) List of variables to iterate over
#     If not specified, the value of .VARIABLES built-in variable is used
#  3. Optional argument to pass when calling the function
# Return:
#     The result of calling the function on each variable.
var_list_map = \
 $(call list_map_transcoded,$1, \
   $(or $(value 2),$(.VARIABLES)),var_name_mangle,var_name_demangle,$(value 3))

##
# Object-oriented version of 'var_list_map'
#
# Params:
#  1. Name of method to invoke on each defined variable, see
#     'var_list_map_invoke_fn'
#  2. Object whichs method to invoke
#  3. (optional) List of variables to iterate over
#     If not specified, the value of .VARIABLES built-in variable is used
#  4. Optional argument to pass when calling the function
# Return:
#     The result of invoking the method on each variable.
var_list_map_invoke = \
 $(call list_map_transcoded_invoke,$1,$2, \
   $(or $(value 3),$(.VARIABLES)),var_name_mangle,var_name_demangle,$(value 4))

##
# Filters variables listed in the first argument from the list in the second
# calling the specified function for each filtered one.
#
# Params:
#  1. Function to call for each occurrence of variable from the first list
#     in the second, see 'var_list_map_fn'
#  2. Variables to check for the match with
#  3. (optional) Variables list being filtered. Defaults to .VARIABLES built-in
#  4. Optional argument to pass when calling the function
# Return:
#     Result of calling the specified function on each filtered variable.
var_list_filter = \
  $(call __var_list_filter,filter,$1,$2,$(or $(value 3), \
      $(.VARIABLES)),$(value 4))

##
# Object-oriented version of 'var_list_filter'
#
# Params:
#  1. Method to invoke for each occurrence of variable from the first list in
#     the second, see 'var_list_map_invoke_fn'
#  2. Object whichs method to invoke
#  3. Variables to check for the match with
#  4. (optional) Variables list being filtered. Defaults to .VARIABLES built-in
#  5. Optional argument to pass when calling the function
# Return:
#     Result of calling the specified function on each filtered variable.
var_list_filter_invoke = \
  $(call __var_list_filter_invoke,filter,$1,$2,$3,$(or $(value 4), \
      $(.VARIABLES)),$(value 5))

##
# Filters out variables listed in the first argument from the list in the
# second calling the specified function for each leftover one.
#
# Params:
#  1. Function to call for each variable of the second list which is not
#     presented in the first one, see 'var_list_map_fn'
#  2. Variables to check for the match with
#  3. (optional) Variables list being filtered. Defaults to .VARIABLES built-in
#  4. Optional argument to pass when calling the function
# Return:
#     Result of calling the specified function on each filtered variable.
var_list_filter_out = \
  $(call __var_list_filter,filter-out,$1,$2,$(or $(value 3), \
      $(.VARIABLES)),$(value 4))

##
# Object-oriented version of 'var_list_filter_out'
#
# Params:
#  1. Method to invoke for each variable of the second list which is not
#     presented in the first one, see 'var_list_map_invoke_fn'
#  2. Object whichs method to invoke
#  3. Variables to check for the match with
#  4. (optional) Variables list being filtered. Defaults to .VARIABLES built-in
#  5. Optional argument to pass when calling the function
# Return:
#     Result of calling the specified function on each filtered variable.
var_list_filter_out_invoke = \
  $(call __var_list_filter_invoke,filter-out,$1,$2,$3,$(or $(value 4), \
      $(.VARIABLES)),$(value 5))

# The real work is done here
# Params:
#  1. Filtering function (filter/filter-out)
#  2. Function to call for each filtered variable
#  3. Variables to check for the match with
#  4. Variables list being filtered
#  5. Argument to pass when calling the function
# Return: result of calling the specified function on each filtered variable
__var_list_filter = $(call list_map_transcoded,$2, \
  $(call $1,$(call var_name_mangle,$3), \
            $(call var_name_mangle,$4)),,var_name_demangle,$5)

# Params:
#  1. Filtering function (filter/filter-out)
#  2. Method
#  3. Object
#  4. Variables to check for the match with
#  5. Variables list being filtered
#  6. Argument to pass when calling the function
# Return: result of calling the specified function on each filtered variable
__var_list_filter_invoke = $(call list_map_transcoded_invoke,$2,$3, \
  $(call $1,$(call var_name_mangle,$4), \
            $(call var_name_mangle,$5)),,var_name_demangle,$6)

endif # __util_var_list_mk
