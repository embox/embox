#
# A simple parser combinator library.
#
#   Date: Oct 5, 2011
# Author: Eldar Abusalimov
#

include mk/core/define.mk
include mk/core/object.mk

#
# Extension: 'parser' builtin function.
#
# $(parser
#     v1,parser1,
#     ...,
#     vN,parserN,
#     result)
#
# Each parser1 .. parserN must be a name of a parser function which takes a
# string as its sole argument and returns a value in form:
#    'singleword_result and the rest of the string'
#
# On each step the result of parsing (the first word) is placed into a variable
# vK and the next parser is invoked.
# Parser K can use values of variables v1 .. vK-1.
#
# If a parser fails to recognize a string it should return an empty (noword)
# string. In such case parsing is terminated and an empty result is returned.
#
# In case if every parser succeeds the last argument ('result') is expanded.
# The result of the expansion MUST be a singleword result.
#
# Usage:
#
#   define parse_something
#   	$(parser \
#   		,parse_any,
#   		a,parse_something_else,
#   		$(new object,Result based on $a value)
#   	)
#   endef
#   $(call def,parse_something)
#
define builtin_func_parser
	$(call builtin_check_min_arity,3)

	$(if $(filter %0 %2 %4 %6 %8,$(words $(builtin_args_list))),
		$(call builtin_error,
			Odd number of arguments to function '$0' expected
		)
	)

	$(and \
		$(call builtin_aux_def,
			$$(call __parser_result,$(builtin_lastarg),$$1)
		)
		$(foreach a,
			# 1 2 3 4 5 -> 3.4 1.2
			$(nofirstword $(call list_reverse,$(call list_fold,
				$(lambda $(subst $2 ,$2.,$1)),
				$(builtin_args_list)$(\s),
				1 3 5 7 9# Odd numbers.
			))),
			$(call builtin_aux_def,
				$$(call __parser_cont,
					$($(basename $a)),
					$$(call $($(subst .,,$(suffix $a))),$$1),
					$(builtin_aux_last))
			)
		),
		# Suppress the value.
	)

	$(builtin_aux_last)
endef
$(call def,builtin_func_parser)

# 1. v.
# 2. Return value of a parser (result + rest string).
# 3. Continuation.
define __parser_cont
	$(warning $0: [$1][$2][$3])
	$(foreach $1,$(firstword $2),
		$(call $3,$(nofirstword $2))
	)
endef
$(call def,__parser_cont)

# 1. Result of parsing.
# 2. Unparsed string.
define __parser_result
	$(warning $0: [$1][$2])
	$(or $(singleword $1),
		$(error \
			Parser must return a singleword result, got '$1' instead
		)
	) $2
endef
$(call def,__parser_result)

define class string
  $.value = $(value 2)
endef

# 1. String
define parse_any
	$1
endef
$(call def,parse_any)

# 1. Parser
# 2. String
define parse_*
	$(or $(call $1,$2),$(call new,string) $2)# XXX
endef
$(call def,parse_*)

# 1. Parser
# 2. String
define parse_?
	$(or $(call $1,$2),$(call new,string) $2)
endef
$(call def,parse_?)

# 1. Predicate
# 2. String
define parse_sat
	$(if $(call $1,$(firstword $2)),$2)
endef
$(call def,parse_sat)

# 1. Char
# 2. String
define parse_char
	$(info $0: [$1] [$2])
	$(if $(eq $1,$(firstword $2)),$2)
endef
$(call def,parse_char)

# 1. String
define parse_string_term
	$(call $(parser \
		,$(fx parse_char,"),
		str,$(fx parse_*,$(fx parse_sat,$(lambda $(not $(eq ",$1))))),
		,$(fx parse_char,"),
		$(str)
	),$1)
endef
$(call def,parse_string_term)

define foo
	$(call $(parser \
		a1,parse_any,# Just consume the first token.
		a2,$(fx parse_?,parse_string_term),
		a3,$(fx parse_char,^),
		{$(a1)} {$($(a2).value)} {$(a3)}$(bar)
	),$1)
endef
$(call def,foo)

bar = $(foreach v,$(filter __obj2%,$(.VARIABLES)), \
  $(warning $v = [$(value $v)]))
$(error [$(value foo)][$(call foo,z " sdfsd " ^)])

