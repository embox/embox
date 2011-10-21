#
# DFA-based lexer.
#
#   Date: Oct 19, 2011
# Author: Eldar Abusalimov
#

ifndef __embuild_lex_mk
__embuild_lex_mk := 1

include mk/core/define.mk

[letter] := \
	A B C D E F G H I J K L M N O P Q R S T U V W X Y Z \
	a b c d e f g h i j k l m n o p q r s t u v w x y z #

[number] := \
	0 1 2 3 4 5 6 7 8 9 #

[alnum]  := \
	$([letter])$([number])

[print]  := \
	$([alnum])! " $$ % & ' ( ) * + , - . / : ; < = > ? @ [ \ ] ^ _ { | } ~ \# #

define builtin_tag_dfa
	$(builtin_aux_alloc)
endef

define builtin_func_dfa
	$(assert $(call builtin_tag,dfa))
	$(call builtin_check_min_arity,1)

	$(foreach tag,$(call builtin_tag,dfa),

		$(call var_assign_recursive_sl,$(tag),
			$${eval $$(__dfa_reset_mk)}
			$$(foreach 1,$$1,$$($(tag)_$$(or $$(__dfa_state),$1)))
		)

		$(tag)
	)
endef

define builtin_func_dfa-state
	$(assert $(call builtin_tag,dfa))
	$(__dfa_check_caller)

	$(call var_assign_recursive_sl,$(call builtin_tag,dfa)_state_$1,
		$$(foreach 2,
			$$(or \
				$(expand $(call list_pairmap,
					$(lambda \
						$(if $2,
							$$$$(if $$$$(filter $$$$1,$$($1)),
									state_$$($2))$(\comma),
							token_$$($1)
						)$$# Suppress a space between elements: '$ ' -> ''.
					),
					$(filter     %0 %2 %4 %6 %8  ,$(builtin_args_list)),
					$(filter-out %0 %2 %4 %6 %8 1,$(builtin_args_list)),
				)$(\s))
				$(if $(filter-out %0 %2 %4 %6 %8,$(words $(builtin_args_list))),
					error
				)
			),
			$${eval $$(__dfa_advance_mk)}
		)
	)
endef

define builtin_func_dfa-token
	$(assert $(call builtin_tag,dfa))
	$(__dfa_check_caller)

	$(call var_assign_recursive_sl,$(call builtin_tag,dfa)_token_$1,
		$${eval $$(__dfa_reset_mk)}
	)
endef

# Do not 'call', only expand.
define __dfa_check_caller
	$(if $(not $(eq dfa,$(builtin_caller))),
		$(call builtin_error,
			Function '$(builtin_name)' must be used within 'dfa' only.
		)
	)
endef

define __dfa_reset_mk
	__dfa_chars :=
	__dfa_state := $1
endef
__dfa_reset_mk := $(value __dfa_reset_mk)

define __dfa_advance_mk
	__dfa_chars += $1
	__dfa_state := $2
endef
__dfa_advance_mk := $(value __dfa_advance_mk)

define aux0_ground
	$(foreach 2,
		$(or \
			$(and $(filter $1,$([whitespace])),ws),
			$(and $(filter $1,"),in_string),
			error
		),
		${eval $(__dfa_advance_mk)}
	)
endef

$(def_all)

define foo
	$(dfa \
		$(dfa-state ground,
			$([whitespace]),ws,
			",in_string
		),
		$(dfa-state ws,
			$([whitespace]),ws,
			token_ws
		),
	)
endef
$(call def,foo)

$(foreach v,$(filter $(foo)%,$(.VARIABLES)),$(warning $v = [$(value $v)]))
$(error done)

define aux0_ground
	$(foreach 2,
		$(or \
			$(and $(filter $1,$([whitespace])),ws),
			$(and $(filter $1,"),in_string),
			error
		),
		${eval $(__dfa_advance_mk)}
	)
endef

define aux0_ws
	$(or \
		$(and $(filter $1,$([whitespace])),ws),
		token_ws,
	)
endef

define aux0
	${eval __dfa_reset_mk}
	$(foreach 1,$1,$($0_$(__dfa_state)))
endef

define xxx

endef

$(def_all)

endif # __embuild_lex_mk
