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
# Compares a string against another for equality.
#
# Params:
#  1. The first string.
#  2. The second one.
# Return:
#     The value if that is the same for both arguments, empty otherwise.
eq = \
	$(findstring $1,$(findstring $2,$1))
builtin_func_eq = $(builtin_to_function_inline)

##
# Builtin function: nowords
# Checks whether the specified string contains no words (nothing but
# whitespaces).
#
# Params:
#  1. The string to check.
# Return:
#     True if the specified text has no non-whitespace characters,
#     false otherwise.
nowords = \
	$(if $(firstword $1),,1)
builtin_func_nowords = $(builtin_to_function_inline)

##
# Builtin function: singleword
# Checks whether the specified string contains exactly one word.
#
# Params:
#  1. The string.
# Return:
#     The word itself if the specified string is a single-word list,
#     nothing (false) otherwise.
singleword = \
	$(if $(word 2,$1),,$(firstword $1))
builtin_func_singleword = $(builtin_to_function_inline)

##
# Builtin function: doubleword
# Checks whether the specified string contains exactly two words.
#
# Params:
#  1. The string.
# Return:
#     The unmodified string if it is a double-word list,
#     nothing (empty string) otherwise.
doubleword = \
	$(if $(filter 2,$(words $1)),$1)
builtin_func_doubleword = $(builtin_to_function_inline)

##
# Builtin function: firstword
# Gets the first word of the specified words list.
#
# Params:
#  1. The target list of words.
# Return:
#     The first word of the list.
firstword = \
	$(firstword $1)# Native builtin.

##
# Builtin function: secondword
# Gets the second word of the specified words list.
#
# Params:
#  1. The target list of words.
# Return:
#     The second word of the list.
secondword = \
	$(word 2,$1)
builtin_func_secondword = $(builtin_to_function_inline)

##
# Builtin function: lastword
# Gets the last word of the specified words list.
#
# Params:
#  1. The target list of words.
# Return:
#     The last word of the list.
lastword = \
	$(lastword $1)# Native builtin.

##
# Builtin function: nofirstword
# Removes the first word from the specified words list.
#
# Params:
#  1. The target list of words
# Return:
#     The list of words with the first one removed.
nofirstword = \
	$(wordlist 2,2147483647,$1)
builtin_func_nofirstword = $(builtin_to_function_inline)

##
# Builtin function: nolastword
# Removes the last word from the specified words list.
#
# Params:
#  1. The target list of words.
# Return:
#     The list of words with the last one removed.
nolastword = \
	$(wordlist 2,$(words $1),x $1)
builtin_func_nolastword = $(builtin_to_function_inline)

##
# Builtin function: words-to
# Gets the list of words ending with word at given index (inclusive).
#
# Params:
#  1. Upper bound.
#  2. The target list of words.
# Return:
#     Words [1 .. arg].
words-to = \
	$(wordlist 1,$1,$2)
builtin_func_words-to = $(builtin_to_function_inline)

##
# Builtin function: words-from
# Gets the list of words starting with word at given index (inclusive).
#
# Params:
#  1. Lower bound.
#  2. The target list of words.
# Return:
#     Words [arg .. nwords].
words-from = \
	$(wordlist $1,2147483647,$2)
builtin_func_words-from = $(builtin_to_function_inline)

##
# Builtin function: trim
# Removes leading and trailing whitespaces.
#
# Params:
#  1. The string.
# Return:
#     The argument with no leading or trailing whitespaces.
trim = \
	$(wordlist 1,2147483647,$1)
builtin_func_trim = $(builtin_to_function_inline)

##
# Builtin function: append
# Appends the second argument after the first using whitespace as a separator
# (if both of the argument are non-empty strings).
#
# Params:
#  1. The first string
#  2. The second string
# Return:
#     The result of string concatenation.
append = \
	$1$(if $2,$(if $1, )$2)
builtin_func_append = $(builtin_to_function_call)

##
# Builtin function: prepend
# Prepends the second argument before the first using whitespace as a separator
# (if both of the argument are non-empty strings).
#
# Params:
#  1. The first string
#  2. The second string
# Return:
#     The result of string concatenation.
prepend = \
	$2$(if $1,$(if $2, )$1)
builtin_func_prepend = $(builtin_to_function_call)

##
# Function: filter-patsubst
# Strict version of patsubst. Leaves only those words that match at least
# one of the specified patterns.
#
# Params:
#  1. Pattern...
#  2. Replacement
#  3. String
# Return:
#     The result of patsubst applied to filtered string.
filter-patsubst = \
	$(foreach __fp,$1,$(patsubst $(__fp),$2,$(filter $(__fp),$3)))
builtin_func_filter-patsubst = $(builtin_to_function_inline)

endif # __core_string_mk
