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
# GMSL String manipulation functions.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __gmsl_string_mk
__gmsl_string_mk := 1

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

__gmsl_characters := A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
__gmsl_characters += a b c d e f g h i j k l m n o p q r s t u v w x y z
__gmsl_characters += 0 1 2 3 4 5 6 7 8 9
__gmsl_characters += ` ~ ! @ \# $$ % ^ & * ( ) - _ = +
__gmsl_characters += { } [ ] \ : ; ' " < > , . / ? |

# ----------------------------------------------------------------------------
# Function:  strlen
# Arguments: 1: A string
# Returns:   Returns the length of the string
# ----------------------------------------------------------------------------
strlen = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(strip $(eval __temp := $(subst $(__gmsl_space),x,$1))$(foreach a,$(__gmsl_characters),$(eval __temp := $$(subst $$a,x,$(__temp))))$(eval __temp := $(subst x,x ,$(__temp)))$(words $(__temp)))

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

endif # __gmsl_string_mk
