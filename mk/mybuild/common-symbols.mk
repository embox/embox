#
#   Date: Mar 12, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
#

#
# Common symbol terminals for Mybuild parsers.
# Note the absence of inclusion guards. This file is included multiple times
# with different values of 'gold_grammar' variable.
#

#
# Each terminal symbol is converted by the corresponding constructor (if any).
# Constructor is a function named '$(gold_grammar)_create-<ID>' (where ID is
# a unique symbol identifier) with the following signature:
#
# Params:
#   1. A list of decimal char codes representing the token.
#
# Return:
#   The converted value, which is then passed as an argument to a production
#   function of the rule containing that symbol in its RHS.
#
# If constructor for some symbol is not defined then the default behavior
# is used, which is to decode an input by replacing all printable characters
# with their values and the rest ones with spaces.
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
	$(for num <- $(gold_default_create),
		$(or $(filter-out 0x%,$(num)),
			$(shell printf '%u' '$(num)')))
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

#
# Optimized contructors for constant terminals (i.e. keywords and punctuation).
#

$(gold_grammar)_create-Comma            := ,
$(gold_grammar)_create-Dot              := .
$(gold_grammar)_create-DotTimes         := .*
$(gold_grammar)_create-LParan           := (
$(gold_grammar)_create-RParan           := )
$(gold_grammar)_create-LBrace           := {
$(gold_grammar)_create-RBrace           := }
$(gold_grammar)_create-Eq               := =
$(gold_grammar)_create-At               := @

#
# For each regular terminal we also define a constant with a human-readable
# description used for error reporting.
#

$(gold_grammar)_name_of-Comma           := ','
$(gold_grammar)_name_of-Dot             := '.'
$(gold_grammar)_name_of-DotTimes        := '.*'
$(gold_grammar)_name_of-LParan          := '('
$(gold_grammar)_name_of-RParan          := ')'
$(gold_grammar)_name_of-LBrace          := '{'
$(gold_grammar)_name_of-RBrace          := '}'
$(gold_grammar)_name_of-Eq              := '='
$(gold_grammar)_name_of-At              := '@'
$(gold_grammar)_name_of-BooleanLiteral  := Boolean Literal
$(gold_grammar)_name_of-NumberLiteral   := Number Literal
$(gold_grammar)_name_of-StringLiteral   := String Literal

$(def_all)

