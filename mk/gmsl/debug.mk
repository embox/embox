#
# Copyright 2008-2010, Mathematics and Mechanics faculty
#                   of Saint-Petersburg State University. All rights reserved.
# Copyright 2008-2010, Lanit-Tercom Inc. All rights reserved.
#
# Copyright 2005-2010 John Graham-Cumming
#
# Please see COPYRIGHT for details.
#

#
# GMSL Debugging facilities.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __gmsl_debug_mk
__gmsl_debug_mk := 1

# ----------------------------------------------------------------------------
# Target:    gmsl-print-%
# Arguments: The % should be replaced by the name of a variable that you
#            wish to print out.
# Action:    Echos the name of the variable that matches the % and its value.
#            For example, 'make gmsl-print-SHELL' will output the value of
#            the SHELL variable
# ----------------------------------------------------------------------------
gmsl-print-%: ; @echo $* = $($*)

# ----------------------------------------------------------------------------
# Function:  assert
# Arguments: 1: A boolean that must be true or the assertion will fail
#            2: The message to print with the assertion
# Returns:   None
# ----------------------------------------------------------------------------
assert = $(if $1,,$(call __gmsl_error,Assertion failure: $2))

# ----------------------------------------------------------------------------
# Function:  assert_exists
# Arguments: 1: Name of file that must exist, if it is missing an assertion
#               will be generated
# Returns:   None
# ----------------------------------------------------------------------------
assert_exists = $(call assert,$(wildcard $1),file '$1' missing)

# ----------------------------------------------------------------------------
# Function:  assert_no_dollar
# Arguments: 1: Name of a function being executd
#            2: Arguments to check
# Returns:   None
# ----------------------------------------------------------------------------
assert_no_dollar = $(call assert,$(call not,$(findstring $$,$2)),$1 called with a dollar sign in argument)

endif # __gmsl_debug_mk
