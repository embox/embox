#
# Extended set of functions for string analysis and manipulation.
#
#   Date: Jan 14, 2011
# Author: Eldar Abusalimov
#

ifndef __core_string_mk
__core_string_mk := 1

##
# Function: nowords
# Checks whether the specified string contains no words (nothing but
# whitespaces).
#
# Params:
#  1. The string to check
# Returns: True if the specified text has no non-whitespace characters,
#          false otherwise
#
nowords = \
  $(call not,$(firstword $1))

##
# Function: singleword
# Checks whether the specified string contains exactly one word.
#
# Params:
#  1. The string to check
# Returns: True if the specified string is a single-word list, false otherwise
#
singleword = \
  $(call make_bool,$(if $(word 2,$1),,$(firstword $1)))

##
# Function: firstword
# Gets the first word of the specified words list.
#
# Params:
#  1. The target list of words
# Returns: the first word of the list
#
firstword = \
  $(firstword $1)# built-in

##
# Function: lastword
# Gets the last word of the specified words list.
#
# Params:
#  1. The target list of words
# Returns: the last word of the list
#
ifeq ($(lastword $(false) $(true)),$(true))
lastword = \
  $(lastword $1)# built-in
else
lastword = \
  $(if $(strip $1),$(word $(words $1),$1))
endif

##
# Function: nofirstword
# Removes the first word from the specified words list.
#
# Params:
#  1. The target list of words
# Returns: Returns the list of words with the first one removed
#
nofirstword = \
  $(wordlist 2,$(words $1),$1)

##
# Function: nolastword
# Removes the last word from the specified words list.
#
# Params:
#  1. The target list of words
# Returns: Returns the list of words with the last one removed
#
nolastword = \
  $(wordlist 2,$(words $1),x $1)


endif # __core_string_mk
