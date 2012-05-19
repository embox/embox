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
# $(call subst-start,from,to,string)
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
# $(call subst-end,from,to,string)
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
# Builtin function: find-start
#
#      $(find-start what,string), or
# $(call find-start,what,string)
#
# Tests if the 'string' starts with 'what' prefix.
#
# Params:
#   1. Prefix to search.
#   2. The string.
# Return:
#   The first argument if it appears to be a prefix of the second one,
#   empty otherwise.
find-start = \
	$(if $(findstring [$2]$1,[$2]$2),$1)
builtin_func-find-start = $(builtin_to_function_call)

##
# Builtin function: find-end
#
#      $(find-end what,string), or
# $(call find-end,what,string)
#
# Tests if the 'string' ends with 'what' suffix.
#
# Params:
#   1. Suffix to search.
#   2. The string.
# Return:
#   The first argument if it appears to be a suffix of the second one,
#   empty otherwise.
find-end = \
	$(if $(findstring $1[$2],$2[$2]),$1)
builtin_func-find-end = $(builtin_to_function_call)

#
# List manipulations.
#

##
# Builtin function: reverse
#
#      $(reverse list...), or
# $(call reverse,list...)
#
# Reverses the specified list.
#
# Params:
#  1. The target list.
# Return:
#   The list with its elements in reverse order.
reverse = \
	$(strip $(call fold,,$1,__reverse_fold))
__reverse_fold = \
	$2 $1
builtin_func-reverse = $(builtin_to_function_inline)

#
# Left folding functions.
#

#
# Implementation note:
#
# Using imperative version (which clobbers context during iterations using
# 'eval') intead of pure functional style implementation (using recursion) is
# much faster (and gives O(n) complexity as seen from GNU Make sources).
# It is about 1500 times faster on folding a list of 10000 elements using
# the identity function as combining.
#

##
# Builtin function: fold
#
#      $(fold initial_value,list...,combining_fn[,optional_args...]), or
# $(call fold,initial_value,list...,combining_fn[,optional_args...])
#
# Takes the first argument and the first item of the list and applies the
# function to them, then feeds the function with this result and the second
# element and so on.
#
# Params:
#   1. Initial value to pass as an intermediate value when calling function
#      for the first time.
#   2. List to iterate over applying the folding function.
#   3. Name of the combining function.
#   4. (optional) argument to pass when calling the function.
# Return:
#   The result of the last function call (if any occurred),
#   or the initial value in case of empty list.
#
# The combining function takes the following arguments:
#   1. Intermediate value obtained as the result of previous function calls
#   2. An element from the list being folded
#   3. Optional argument (if any)
# Return:
#   The value to pass to the next function call as new intermediate value, if
#   there are more elements in the list.
#   Otherwise this value is used as the return value of fold.
#
# See: scan, which preserves intermediate results.
#
fold = \
	$(and ${eval __fold_tmp__ := $$1} \
		$(foreach 2,$2,${eval __fold_tmp__ := \
			$$(call $$3,$$(__fold_tmp__),$$2,$$(value 4))}),)$(__fold_tmp__)
builtin_func-fold = $(builtin_to_function_call)

##
# Builtin function: scan
#
#      $(scan initial_value,list...,combining_fn[,optional_args...]), or
# $(call scan,initial_value,list...,combining_fn[,optional_args...])
#
# Like 'fold' excepting that each intermediate result is returned (not only
# the last one).
#
# Params:
#   See 'fold'.
# Return:
#   The list of intermediate and final results of the function calls (if any
#   occurred),
#   or the initial value in case of empty list.
scan = \
	${eval __fold_tmp__ := \
		$$1}$(foreach 2,$2,$(__fold_tmp__)${eval __fold_tmp__ := \
				$$(call $$3,$$(__fold_tmp__),$$2,$$(value 4))})$(if \
			$(firstword $2), )$(__fold_tmp__)
builtin_func-scan = $(builtin_to_function_call)

__fold_tmp__ :=
__cache_transient += __fold_tmp__

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
# Function: r-patsubst
#
#      $(r-patsubst pattern,replacement,text), or
# $(call r-patsubst,pattern,replacement,text)
#
# Recursive 'patsubst'. Unlike regular 'patsubst' this one performs pattern
# replacement until at least one of the words in target expression matches the
# pattern.
#
# Example:
#          $(r-patsubst %/,%,foo/ bar/// baz) produces 'foo bar baz'
#   whilst   $(patsubst %/,%,foo/ bar/// baz) is just  'foo bar// baz'
#
# Params:
#   1. Pattern.
#   2. Replacement.
#   3. String.
# Return:
#   The result of 'patsubst' being applied while the value matches the pattern.
r-patsubst = \
	$(if $(filter $1,$3),$(call $0,$1,$2,$(patsubst $1,$2,$3)),$3)
builtin_func-r-patsubst = $(builtin_to_function_call)

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
