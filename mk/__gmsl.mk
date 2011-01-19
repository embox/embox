# ----------------------------------------------------------------------------
#
# GNU Make Standard Library (GMSL)
#
# A library of functions to be used with GNU Make's $(call) that
# provides functionality not available in standard GNU Make.
#
# Copyright (c) 2005-2010 John Graham-Cumming
#
# This file is part of GMSL
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# Neither the name of the John Graham-Cumming nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# ----------------------------------------------------------------------------

include util/__gmsl.mk

# ###########################################################################
# LOGICAL OPERATORS
# ###########################################################################

# not is defined in gmsl

# ----------------------------------------------------------------------------
# Function:  and
# Arguments: Two boolean values
# Returns:   Returns $(true) if both of the booleans are true
# ----------------------------------------------------------------------------
ifneq ($(__gmsl_have_and),$(true))
and = $(__gmsl_tr2)$(if $1,$(if $2,$(true),$(false)),$(false))
endif

# ----------------------------------------------------------------------------
# Function:  or
# Arguments: Two boolean values
# Returns:   Returns $(true) if either of the booleans is true
# ----------------------------------------------------------------------------
ifneq ($(__gmsl_have_or),$(true))
or = $(__gmsl_tr2)$(if $1$2,$(true),$(false))
endif

# ----------------------------------------------------------------------------
# Function:  xor
# Arguments: Two boolean values
# Returns:   Returns $(true) if exactly one of the booleans is true
# ----------------------------------------------------------------------------
xor = $(__gmsl_tr2)$(if $1,$(if $2,$(false),$(true)),$(if $2,$(true),$(false)))

# ----------------------------------------------------------------------------
# Function:  nand
# Arguments: Two boolean values
# Returns:   Returns value of 'not and'
# ----------------------------------------------------------------------------
nand = $(__gmsl_tr2)$(if $1,$(if $2,$(false),$(true)),$(true))

# ----------------------------------------------------------------------------
# Function:  nor
# Arguments: Two boolean values
# Returns:   Returns value of 'not or'
# ----------------------------------------------------------------------------
nor = $(__gmsl_tr2)$(if $1$2,$(false),$(true))

# ----------------------------------------------------------------------------
# Function:  xnor
# Arguments: Two boolean values
# Returns:   Returns value of 'not xor'
# ----------------------------------------------------------------------------
xnor =$(__gmsl_tr2)$(if $1,$(if $2,$(true),$(false)),$(if $2,$(false),$(true)))

# ###########################################################################
# LIST MANIPULATION FUNCTIONS
# ###########################################################################

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
map = $(__gmsl_tr2)$(call list_map,$1,$2)

# ----------------------------------------------------------------------------
# Function:  pairmap
# Arguments: 1: Name of function to $(call) for each pair of elements
#            2: List to iterate over calling the function in 1
#            3: Second list to iterate over calling the function in 1
# Returns:   The list after calling the function on each pair of elements
# ----------------------------------------------------------------------------
pairmap = $(__gmsl_tr3)$(call list_pairmap,$1,$2,$3)

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

# ###########################################################################
# STRING MANIPULATION FUNCTIONS
# ###########################################################################

# ----------------------------------------------------------------------------
# Function:  seq
# Arguments: 1: A string to compare against...
#            2: ...this string
# Returns:   Returns $(true) if the two strings are identical
# ----------------------------------------------------------------------------
seq = $(__gmsl_tr2)$(if $1,$(if $2,$(if $(filter-out xx,x$(subst $1,,$2)$(subst $2,,$1)x),$(false),$(true)),$(call not,$(call __gmsl_make_bool,$1))),$(call not,$(call __gmsl_make_bool,$2)))

# ----------------------------------------------------------------------------
# Function:  sne
# Arguments: 1: A string to compare against...
#            2: ...this string
# Returns:   Returns $(true) if the two strings are not the same
# ----------------------------------------------------------------------------
sne = $(__gmsl_tr2)$(call not,$(call seq,$1,$2))

# ----------------------------------------------------------------------------
# Function:  split
# Arguments: 1: The character to split on
#            2: A string to split
# Returns:   Splits a string into a list separated by spaces at the split
#            character in the first argument
# ----------------------------------------------------------------------------
split = $(__gmsl_tr2)$(strip $(subst $1, ,$2))

# ----------------------------------------------------------------------------
# Function:  merge
# Arguments: 1: The character to put between fields
#            2: A list to merge into a string
# Returns:   Merges a list into a single string, list elements are separated
#            by the character in the first argument
# ----------------------------------------------------------------------------
merge = $(__gmsl_tr2)$(strip $(if $2,                                     \
            $(if $(call seq,1,$(words $2)),                               \
                $2,$(call first,$2)$1$(call merge,$1,$(call rest,$2)))))

ifdef __gmsl_have_eval
# ----------------------------------------------------------------------------
# Function:  tr
# Arguments: 1: The list of characters to translate from
#            2: The list of characters to translate to
#            3: The text to translate
# Returns:   Returns the text after translating characters
# ----------------------------------------------------------------------------
tr = $(strip $(__gmsl_tr3)$(call assert_no_dollar,$0,$1$2$3)              \
     $(eval __gmsl_t := $3)                                               \
     $(foreach c,                                                         \
         $(join $(addsuffix :,$1),$2),                                    \
         $(eval __gmsl_t :=                                               \
             $(subst $(word 1,$(subst :, ,$c)),$(word 2,$(subst :, ,$c)), \
                 $(__gmsl_t))))$(__gmsl_t))

# Common character classes for use with the tr function.  Each of
# these is actually a variable declaration and must be wrapped with
# $() or ${} to be used.

[A-Z] := A B C D E F G H I J K L M N O P Q R S T U V W X Y Z #
[a-z] := a b c d e f g h i j k l m n o p q r s t u v w x y z #
[0-9] := 0 1 2 3 4 5 6 7 8 9 #
[A-F] := A B C D E F #

# ----------------------------------------------------------------------------
# Function:  uc
# Arguments: 1: Text to upper case
# Returns:   Returns the text in upper case
# ----------------------------------------------------------------------------
uc = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(call tr,$([a-z]),$([A-Z]),$1)

# ----------------------------------------------------------------------------
# Function:  lc
# Arguments: 1: Text to lower case
# Returns:   Returns the text in lower case
# ----------------------------------------------------------------------------
lc = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(call tr,$([A-Z]),$([a-z]),$1)

# ----------------------------------------------------------------------------
# Function:  strlen
# Arguments: 1: A string
# Returns:   Returns the length of the string
# ----------------------------------------------------------------------------
__gmsl_characters := A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
__gmsl_characters += a b c d e f g h i j k l m n o p q r s t u v w x y z
__gmsl_characters += 0 1 2 3 4 5 6 7 8 9
__gmsl_characters += ` ~ ! @ \# $$ % ^ & * ( ) - _ = +
__gmsl_characters += { } [ ] \ : ; ' " < > , . / ? |

# Aside: if you read the above you might think that the lower-case
# letter x is missing, and that that's an error.  It is missing, but
# it's not an error.  __gmsl_characters is used by the strlen
# function.  strlen works by transforming every character and space
# into the letter x and then counting the x's.  Since there's no need
# to transform x into x I omitted it.

# This results in __gmsl_space containing just a space

__gmsl_space :=
__gmsl_space +=

strlen = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(strip $(eval __temp := $(subst $(__gmsl_space),x,$1))$(foreach a,$(__gmsl_characters),$(eval __temp := $$(subst $$a,x,$(__temp))))$(eval __temp := $(subst x,x ,$(__temp)))$(words $(__temp)))

# This results in __gmsl_newline containing just a newline

define __gmsl_newline


endef

# This results in __gmsl_tab containing a tab

__gmsl_tab :=	#

# ----------------------------------------------------------------------------
# Function:  substr
# Arguments: 1: A string
# 	     2: Start position (first character is 1)
#	     3: End position (inclusive)
# Returns:   A substring.
# Note:      The string in $1 must not contain a �
# ----------------------------------------------------------------------------

substr = $(__gmsl_tr3)$(call assert_no_dollar,$0,$1$2$3)$(strip $(eval __temp := $$(subst $$(__gmsl_space),� ,$$1))$(foreach a,$(__gmsl_characters),$(eval __temp := $$(subst $$a,$$a$$(__gmsl_space),$(__temp))))$(eval __temp := $(wordlist $2,$3,$(__temp))))$(subst �,$(__gmsl_space),$(subst $(__gmsl_space),,$(__temp)))

endif # __gmsl_have_eval

# ###########################################################################
# SET MANIPULATION FUNCTIONS
# ###########################################################################

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

# ###########################################################################
# ARITHMETIC LIBRARY
# ###########################################################################

include util/math.mk

ifdef __gmsl_have_eval
# ###########################################################################
# ASSOCIATIVE ARRAYS
# ###########################################################################

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

ifdef __gmsl_have_eval
# ###########################################################################
# NAMED STACKS
# ###########################################################################

# ----------------------------------------------------------------------------
# Function:  push
# Arguments: 1: Name of stack
#            2: Value to push onto the top of the stack (must not contain
#               a space)
# Returns:   None
# ----------------------------------------------------------------------------
push = $(__gmsl_tr2)$(call assert_no_dollar,$0,$1$2)$(eval __gmsl_stack_$1 := $2 $(if $(filter-out undefined,\
    $(origin __gmsl_stack_$1)),$(__gmsl_stack_$1)))

# ----------------------------------------------------------------------------
# Function:  pop
# Arguments: 1: Name of stack
# Returns:   Top element from the stack after removing it
# ----------------------------------------------------------------------------
pop = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(strip $(if $(filter-out undefined,$(origin __gmsl_stack_$1)), \
    $(call first,$(__gmsl_stack_$1))                                       \
    $(eval __gmsl_stack_$1 := $(call rest,$(__gmsl_stack_$1)))))

# ----------------------------------------------------------------------------
# Function:  peek
# Arguments: 1: Name of stack
# Returns:   Top element from the stack without removing it
# ----------------------------------------------------------------------------
peek = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(call first,$(__gmsl_stack_$1))

# ----------------------------------------------------------------------------
# Function:  depth
# Arguments: 1: Name of stack
# Returns:   Number of items on the stack
# ----------------------------------------------------------------------------
depth = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(words $(__gmsl_stack_$1))

endif # __gmsl_have_eval

# ###########################################################################
# DEBUGGING FACILITIES
# ###########################################################################

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
