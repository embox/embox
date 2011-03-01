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
# GMSL Set manipulation functions.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __gmsl_set_mk
__gmsl_set_mk := 1

# Sets are represented by sorted, deduplicated lists.  To create a set
# from a list use set_create, or start with the empty_set and
# set_insert individual elements

# This is the empty set
empty_set :=

# ----------------------------------------------------------------------------
# Function:  set_create
# Arguments: 1: A list of set elements
# Returns:   Returns the newly created set
# ----------------------------------------------------------------------------
set_create = $(__gmsl_tr1)$(sort $1)

# ----------------------------------------------------------------------------
# Function:  set_insert
# Arguments: 1: A single element to add to a set
#            2: A set
# Returns:   Returns the set with the element added
# ----------------------------------------------------------------------------
set_insert = $(__gmsl_tr2)$(sort $1 $2)

# ----------------------------------------------------------------------------
# Function:  set_remove
# Arguments: 1: A single element to remove from a set
#            2: A set
# Returns:   Returns the set with the element removed
# ----------------------------------------------------------------------------
set_remove = $(__gmsl_tr2)$(filter-out $1,$2)

# ----------------------------------------------------------------------------
# Function:  set_is_member
# Arguments: 1: A single element
#            2: A set
# Returns:   Returns $(true) if the element is in the set
# ----------------------------------------------------------------------------
set_is_member = $(__gmsl_tr2)$(if $(filter $1,$2),$(true),$(false))

# ----------------------------------------------------------------------------
# Function:  set_union
# Arguments: 1: A set
#            2: Another set
# Returns:   Returns the union of the two sets
# ----------------------------------------------------------------------------
set_union = $(__gmsl_tr2)$(sort $1 $2)

# ----------------------------------------------------------------------------
# Function:  set_intersection
# Arguments: 1: A set
#            2: Another set
# Returns:   Returns the intersection of the two sets
# ----------------------------------------------------------------------------
set_intersection = $(__gmsl_tr2)$(filter $1,$2)

# ----------------------------------------------------------------------------
# Function:  set_is_subset
# Arguments: 1: A set
#            2: Another set
# Returns:   Returns $(true) if the first set is a subset of the second
# ----------------------------------------------------------------------------
set_is_subset = $(__gmsl_tr2)$(call set_equal,$(call set_intersection,$1,$2),$1)

# ----------------------------------------------------------------------------
# Function:  set_equal
# Arguments: 1: A set
#            2: Another set
# Returns:   Returns $(true) if the two sets are identical
# ----------------------------------------------------------------------------
set_equal = $(__gmsl_tr2)$(call seq,$1,$2)

endif # __gmsl_set_mk
