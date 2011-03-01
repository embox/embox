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
# GMSL Associative arrays.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __gmsl_map_mk
__gmsl_map_mk := 1

ifdef __gmsl_have_eval

# ----------------------------------------------------------------------------
# Function:  set
# Arguments: 1: Name of associative array
#            2: The key value to associate
#            3: The value associated with the key
# Returns:   None
# ----------------------------------------------------------------------------
set = $(__gmsl_tr3)$(call assert_no_dollar,$0,$1$2$3)$(eval __gmsl_aa_$1_$(__gmsl_space)_$2_gmsl_aa_$1 = $3)

# ----------------------------------------------------------------------------
# Function:  get
# Arguments: 1: Name of associative array
#            2: The key to retrieve
# Returns:   The value stored in the array for that key
# ----------------------------------------------------------------------------
get = $(strip $(__gmsl_tr2)$(call assert_no_dollar,$0,$1$2)$(__gmsl_aa_$1_$(__gmsl_space)_$2_gmsl_aa_$1))

# ----------------------------------------------------------------------------
# Function:  keys
# Arguments: 1: Name of associative array
# Returns:   Returns a list of all defined keys in the array
# ----------------------------------------------------------------------------
keys = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(sort $(patsubst _%_gmsl_aa_$1,%, \
                  $(filter _%_gmsl_aa_$1,$(.VARIABLES))))

# ----------------------------------------------------------------------------
# Function:  defined
# Arguments: 1: Name of associative array
#            2: The key to test
# Returns:   Returns true if the key is defined (i.e. not empty)
# ----------------------------------------------------------------------------
defined = $(__gmsl_tr2)$(call assert_no_dollar,$0,$1$2)$(call sne,$(call get,$1,$2),)

endif # __gmsl_have_eval

endif # __gmsl_map_mk
