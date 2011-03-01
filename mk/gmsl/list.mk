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
# GMSL List manipulation functions.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __gmsl_list_mk
__gmsl_list_mk := 1

include util/list.mk

# ----------------------------------------------------------------------------
# Function:  first (same as LISP's car, or head)
# Arguments: 1: A list
# Returns:   Returns the first element of a list
# ----------------------------------------------------------------------------
first = $(__gmsl_tr1)$(call firstword,$1)

# ----------------------------------------------------------------------------
# Function:  last
# Arguments: 1: A list
# Returns:   Returns the last element of a list
# ----------------------------------------------------------------------------
last = $(__gmsl_tr1)$(call lastword,$1)

# ----------------------------------------------------------------------------
# Function:  rest (same as LISP's cdr, or tail)
# Arguments: 1: A list
# Returns:   Returns the list with the first element removed
# ----------------------------------------------------------------------------
rest = $(__gmsl_tr1)$(call nofirstword,$1)

# ----------------------------------------------------------------------------
# Function:  chop
# Arguments: 1: A list
# Returns:   Returns the list with the last element removed
# ----------------------------------------------------------------------------
chop = $(__gmsl_tr1)$(call nolastword,$1)

# ----------------------------------------------------------------------------
# Function:  map
# Arguments: 1: Name of function to $(call) for each element of list
#            2: List to iterate over calling the function in 1
# Returns:   The list after calling the function on each element
# ----------------------------------------------------------------------------
map = $(__gmsl_tr2)$(strip $(call list_map,$1,$2))

# ----------------------------------------------------------------------------
# Function:  pairmap
# Arguments: 1: Name of function to $(call) for each pair of elements
#            2: List to iterate over calling the function in 1
#            3: Second list to iterate over calling the function in 1
# Returns:   The list after calling the function on each pair of elements
# ----------------------------------------------------------------------------
pairmap = $(__gmsl_tr3)$(strip $(call list_pairmap,$1,$2,$3))

# ----------------------------------------------------------------------------
# Function:  leq
# Arguments: 1: A list to compare against...
#            2: ...this list
# Returns:   Returns $(true) if the two lists are identical
# ----------------------------------------------------------------------------
leq = $(__gmsl_tr2)$(call list_equal,$1,$2)

# ----------------------------------------------------------------------------
# Function:  lne
# Arguments: 1: A list to compare against...
#            2: ...this list
# Returns:   Returns $(true) if the two lists are different
# ----------------------------------------------------------------------------
lne = $(__gmsl_tr2)$(call not,$(call list_equal,$1,$2))

# ----------------------------------------------------------------------------
# Function:  reverse
# Arguments: 1: A list to reverse
# Returns:   The list with its elements in reverse order
# ----------------------------------------------------------------------------
reverse = $(__gmsl_tr1)$(call list_reverse,$1)

# ----------------------------------------------------------------------------
# Function:  uniq
# Arguments: 1: A list from which to remove repeated elements
# Returns:   The list with duplicate elements removed without reordering
# ----------------------------------------------------------------------------
uniq = $(strip $(__gmsl_tr1)$(if $1,$(call uniq,$(call chop,$1)) \
            $(if $(filter $(call last,$1),$(call chop,$1)),,$(call last,$1))))

# ----------------------------------------------------------------------------
# Function:  length
# Arguments: 1: A list
# Returns:   The number of elements in the list
# ----------------------------------------------------------------------------
length = $(__gmsl_tr1)$(words $1)

endif # __gmsl_list_mk
