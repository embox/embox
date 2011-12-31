#
# Extended set of functions for string analysis and manipulation.
#
#   Date: Jan 14, 2011
# Author: Eldar Abusalimov
#

ifndef __core_string_mk
__core_string_mk := 1

##
# Builtin function: eq
#
#      $(eq first...,second...), or
# $(call eq,first...,second...)
#
# Params:
#   1. The first string.
#   2. The second one.
# Return:
#   The value if that is the same for both arguments, empty otherwise.
eq = \
	$(findstring $1,$(findstring $2,$1))
builtin_func-eq = $(builtin_to_function_inline)

##
# Builtin function: nowords
#
#      $(nowords words...), or
# $(call nowords,words...)
#
# Checks whether the specified string contains no words (nothing but
# whitespaces).
#
# Params:
#   1. The string to check.
# Return:
#   True if the specified text has no non-whitespace characters,
#   false otherwise.
nowords = \
	$(if $(firstword $1),,1)
builtin_func-nowords = $(builtin_to_function_inline)

##
# Builtin function: singleword
#
#      $(singleword words...), or
# $(call singleword,words...)
#
# Checks whether the specified string contains exactly one word.
#
# Params:
#   1. The string.
# Return:
#   The word itself if the specified string is a single-word list,
#   nothing (false) otherwise.
singleword = \
	$(if $(word 2,$1),,$(firstword $1))
builtin_func-singleword = $(builtin_to_function_inline)

##
# Builtin function: doubleword
#
#      $(doubleword words...), or
# $(call doubleword,words...)
#
# Checks whether the specified string contains exactly two words.
#
# Params:
#   1. The string.
# Return:
#   The unmodified string if it is a double-word list,
#   nothing (empty string) otherwise.
doubleword = \
	$(if $(findstring $(words $1),2),$1)
builtin_func-doubleword = $(builtin_to_function_inline)

##
# Builtin function: multiword
#
#      $(multiword words...), or
# $(call multiword,words...)
#
# Checks whether the specified string contains at least two words.
#
# Params:
#   1. The string.
# Return:
#   The unmodified string if it is a multi-word list,
#   nothing (empty string) otherwise.
multiword = \
	$(if $(word 2,$1),$1)
builtin_func-multiword = $(builtin_to_function_inline)

##
# Builtin function: firstword
#
#      $(firstword words...), or
# $(call firstword,words...)
#
# Gets the first word of the specified words list.
#
# Params:
#   1. The target list of words.
# Return:
#   The first word of the list.
firstword = \
	$(firstword $1)# Native builtin.

##
# Builtin function: secondword
#
#      $(secondword words...), or
# $(call secondword,words...)
#
# Gets the second word of the specified words list.
#
# Params:
#   1. The target list of words.
# Return:
#   The second word of the list.
secondword = \
	$(word 2,$1)
builtin_func-secondword = $(builtin_to_function_inline)

##
# Builtin function: lastword
#
#      $(lastword words...), or
# $(call lastword,words...)
#
# Gets the last word of the specified words list.
#
# Params:
#   1. The target list of words.
# Return:
#   The last word of the list.
lastword = \
	$(lastword $1)# Native builtin.

##
# Builtin function: nofirstword
#
#      $(nofirstword words...), or
# $(call nofirstword,words...)
#
# Removes the first word from the specified words list.
#
# Params:
#   1. The target list of words
# Return:
#   The list of words with the first one removed.
nofirstword = \
	$(wordlist 2,2147483647,$1)
builtin_func-nofirstword = $(builtin_to_function_inline)

##
# Builtin function: nolastword
#
#      $(nolastword words...), or
# $(call nolastword,words...)
#
# Removes the last word from the specified words list.
#
# Params:
#   1. The target list of words.
# Return:
#   The list of words with the last one removed.
nolastword = \
	$(wordlist 2,$(words $1),x $1)
builtin_func-nolastword = $(builtin_to_function_inline)

##
# Builtin function: words-to
#
#      $(words-to words...), or
# $(call words-to,words...)
#
# Gets the list of words ending with word at given index (inclusive).
#
# Params:
#   1. Upper bound.
#   2. The target list of words.
# Return:
#   Words [1 .. arg].
words-to = \
	$(wordlist 1,$1,$2)
builtin_func-words-to = $(builtin_to_function_inline)

##
# Builtin function: words-from
#
#      $(words-from words...), or
# $(call words-from,words...)
#
# Gets the list of words starting with word at given index (inclusive).
#
# Params:
#   1. Lower bound.
#   2. The target list of words.
# Return:
#   Words [arg .. nwords].
words-from = \
	$(wordlist $1,2147483647,$2)
builtin_func-words-from = $(builtin_to_function_inline)

##
# Builtin function: trim
#
#      $(trim string), or
# $(call trim,string)
#
# Removes leading and trailing whitespaces.
#
# Params:
#   1. The string.
# Return:
#   The argument with no leading or trailing whitespaces.
trim = \
	$(wordlist 1,2147483647,$1)
builtin_func-trim = $(builtin_to_function_inline)

##
# Builtin function: subst-start
#
#      $(subst-start from,to,string), or
# $(call subst-start from,to,string)
#
# Performs a string substitution of 'from' in case when the 'string' start
# with it.
#
# Params:
#   1. Prefix to replace.
#   2. Replacement.
#   3. The string.
# Return:
#   The string with prefix (if any) replaced.
subst-start = \
	$(if $(findstring [$3]$1,[$3]$3),$(subst [$3]$1,$2,[$3]$3),$3)
builtin_func-subst-start = $(builtin_to_function_call)

##
# Builtin function: subst-end
#
#      $(subst-end from,to,string), or
# $(call subst-end from,to,string)
#
# Performs a string substitution of 'from' in case when the 'string' ends
# with it.
#
# Params:
#   1. Suffix to replace.
#   2. Replacement.
#   3. The string.
# Return:
#   The string with suffix (if any) replaced.
subst-end = \
	$(if $(findstring $1[$3],$3[$3]),$(subst $1[$3],$2,$3[$3]),$3)
builtin_func-subst-end = $(builtin_to_function_call)

##
# Builtin function: append
# Appends the second argument after the first using whitespace as a separator
# (if both of the argument are non-empty strings).
#
# Params:
#   1. The first string
#   2. The second string
# Return:
#   The result of string concatenation.
append = \
	$1$(if $2,$(if $1, )$2)
builtin_func-append = $(builtin_to_function_call)

##
# Builtin function: prepend
# Prepends the second argument before the first using whitespace as a separator
# (if both of the argument are non-empty strings).
#
# Params:
#   1. The first string
#   2. The second string
# Return:
#    The result of string concatenation.
prepend = \
	$2$(if $1,$(if $2, )$1)
builtin_func-prepend = $(builtin_to_function_call)

##
# Function: filter-patsubst
#
#      $(filter-patsubst pattern...,replacement,text), or
# $(call filter-patsubst,pattern...,replacement,text)
#
# Strict version of patsubst. Leaves only those words that match at least
# one of the specified patterns. Words that match more than one pattern will
# be repeated for each match.
#
# Params:
#   1. Pattern...
#   2. Replacement.
#   3. String.
# Return:
#   The result of patsubst applied to filtered string.
filter-patsubst = \
	$(foreach __fp,$1,$(patsubst $(__fp),$2,$(filter $(__fp),$3)))
builtin_func-filter-patsubst = $(builtin_to_function_inline)

##
# Builtin function: silent-foreach
#
#      $(silent-foreach var,list,text)
#
# A version of 'foreach' that iterates over the list as usual, but suppresses
# the output of expansions of 'text' and returns nothing.
#
# Params:
#   1. Iteration variable.
#   2. The list to iterate over.
#   3. Text to expand for each word in the list.
# Return:
#   Empty.
builtin_func-silent-foreach = \
	$(call builtin_check_min_arity,3)$$(if $$(foreach $(builtin_args)),)

silent-foreach =# Nothing (stub).

endif # __core_string_mk
