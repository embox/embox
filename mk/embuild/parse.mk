#
# A simple parser combinator library.
#
#   Date: Oct 5, 2011
# Author: Eldar Abusalimov
#


#
# Extension: 'parse' builtin function.
#
# $(parse
#     tokens...,
#     v1,parser1,
#     ...,
#     vN,parserN,
#     result)
#
# Each 'parserX' and the 'result' parsers must return a value in form:
#    'single_word_result and the rest of the string'
# On each step the result of parsing (the first word) is placed into a variable
# vX and the next parser is called with the rest tokens as the only argument.
# Parser N can use values of variables v1 ... vN-1.
# If a parser fails to recognize a string it should return an empty (noword)
# string. In such case parsing is terminated and an empty result is returned.
#
# Usage:
#
#   define parse_something
#   	$(parse list of tokens,
#   		,$1,# Just consume the first token.
#   		a,$(call parse_something_else,$1),
#   		$(new object,Result based on $a value) $1
#   	)
#   endef
#   $(call def,parse_something)
#
define builtin_func_parse
	$(call builtin_check_min_arity,2)

	$(if $(filter-out %0 %2 %4 %6 %8,$(words $(builtin_args_list))),
		$(call builtin_error,
			Even number of arguments to function '$0' expected
		)
	)

	$(and \
		$(call builtin_aux_def,$(builtin_lastarg))
		$(foreach a,
			# 1 2 3 4 5 6 -> 4.5 2.3
			$(nofirstword $(call list_reverse,$(call list_fold,
				$(lambda $(subst $2 ,$2.,$1)),
				$(nofirstword $(builtin_args_list)) x,
				0 2 4 6 8# Even numbers.
			))),
			$(call builtin_aux_def,
				$$(call __parse_cont,
					$($(basename $a)),
					$($(subst .,,$(suffix $a))),
					$(builtin_aux_last))
			)
		),# Suppress the value.
	)

	$$(call $(builtin_aux_last),$1)
endef
$(call def,builtin_func_parse)

define __parse_cont
	$(warning $0: [$1][$2][$3])
	$(foreach $1,$(firstword $2),
		$(call $3,$(nofirstword $2))
	)
endef
$(call def,__parse_cont)

$(foreach v,$(filter __builtin_aux%_foo,$(.VARIABLES)), \
  $(warning $v = [$(value $v)]))
$(error [$(value foo)][$(foo)])

