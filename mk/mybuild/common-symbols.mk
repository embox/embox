#
# Common symbol terminals for parsers
#

#
# Each symbol is converted by the corresponding constructor (if any has been
# defined). Constructor is a function named '$(gold_grammar)_create-<ID>'
# (where ID is a unique symbol identifier) with the following signature:
#
# Params:
#   1. For terminals: a list of decimal char codes representing the token.
#      For nonterminals: the result of production.
#
# Return:
#   Converted value. The value is then passed to a rule containing that symbol
#   in its RHS or returned to user in case of the Start Symbol.
#
# If constructor for some symbol is not defined then the default behavior
# is used:
#   For terminals:
#     Decodes an input by replacing all printable characters with their values
#     and the rest ones with spaces.
#   For nonterminals:
#     Outputs the result of production as is, without modifying it.
#
# Constructor may also use a special 'gold_default_create' function to get
# the default value.
#

# Symbol: BooleanLiteral
define $(gold_grammar)_create-BooleanLiteral
	$(if $(filter true,$(gold_default_create)),1)
endef

# Symbol: Identifier
define $(gold_grammar)_create-Identifier
	$(gold_default_create:^%=%)
endef

# Symbol: NumberLiteral
define $(gold_grammar)_create-NumberLiteral
	$(or \
		$(filter-out 0x%,$(gold_default_create)),
		$(call gold_report_warning,
				NYI: Hex literals)
	)
endef

# Symbol: StringLiteral
define $(gold_grammar)_create-StringLiteral
	$(subst \\ ,\,# Unescape '\' sequences.
		$(subst \',',
			$(if $(eq ",$(call gold_default_create,$(firstword $1))),
				# Regular "..." string.
				$(subst \",",$(subst \t,$(\t),$(subst \n,$(\n),$(subst \r,,
					$(subst \\,\\ ,
						$(call gold_default_create,
							# Remove '"' at the ends.
							$(wordlist 3,$(words $1),x $1)
						)
					)
				)))),
				# Verbatim '''...''' string.
				$(subst \\,\\ ,
					$(call gold_default_create,
						# Remove ''' at the ends.
						$(wordlist 7,$(words $1),x x x $1)
					)
				)
			)
		)
	)
endef

$(def_all)

