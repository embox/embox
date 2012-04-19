#
#   Date: Apr 18, 2012
# Author: Eldar Abusalimov
#

#
# Common parser rules for Mybuild parsers.
# There are no inclusion guards because this file is included multiple times
# with different values of 'gold_grammar' variable.
#

#
# As for symbols each rule can have a constructor that is used to produce an
# application-specific representation of the rule data.
# Production functions are named '$(gold_grammar)_produce-<ID>' and have the
# following signature:
#
# Params:
#   1..N: Each argument contains a value of the corresponding symbol in the RHS
#         of the rule production.
#
# Return:
#   The value to pass as an argument to a rule containing the production
#   of this rule in its RHS, or to return to user in case of the Start Symbol.
#
# If production function is not defined then the rule is produced by
# concatenating the RHS through spaces. To reuse this default value one can
# call 'gold_default_produce' function.
#

# Rule: <Annotation> ::= '@' <Reference> <AnnotationInitializer>
define $(gold_grammar)_produce-Annotation_At
	$(for annotation <- $(new MyAnnotation),
		$(set annotation->type_link,$2)
		$(set annotation->bindings,$3)
		$(annotation))
endef

# Rule: <AnnotationInitializer> ::= '(' <ParametersList> ')'
$(gold_grammar)_produce-AnnotationInitializer_LParan_RParan = $2

# Rule: <AnnotationInitializer> ::= '(' <Value> ')'
define $(gold_grammar)_produce-AnnotationInitializer_LParan_RParan2
	$(for binding<-$(new MyOptionBinding),
		$(set binding->option_link,$(new ELink,value,$(gold_location)))
		$(set binding->optionValue,$2)
		$(binding))
endef

# Rule: <ParametersList> ::= <Parameter> ',' <ParametersList>
$(gold_grammar)_produce-ParametersList_Comma = $1 $3

# Rule: <Parameter> ::= <SimpleReference> '=' <Value>
define $(gold_grammar)_produce-Parameter_Eq
	$(for binding<-$(new MyOptionBinding),
		$(set binding->option_link,$1)
		$(set binding->optionValue,$3)
		$(binding))
endef

# Rule: <Value> ::= StringLiteral
$(gold_grammar)_produce-Value_StringLiteral  = $(new MyStringOptionValue,$1)
# Rule: <Value> ::= NumberLiteral
$(gold_grammar)_produce-Value_NumberLiteral  = $(new MyNumberOptionValue,$1)
# Rule: <Value> ::= BooleanLiteral
$(gold_grammar)_produce-Value_BooleanLiteral = $(new MyBooleanOptionValue,$1)
# Rule: <Value> ::= <Reference>
define $(gold_grammar)_produce-Value
	$(for value<-$(new MyTypeReferenceOptionValue),
		$(set value->value_link,$1)
		$(value))
endef

# Rule: <Reference> ::= <QualifiedName>
$(gold_grammar)_produce-Reference                  = $(new ELink,$1,$(gold_location))
# Rule: <SimpleReference> ::= Identifier
$(gold_grammar)_produce-SimpleReference_Identifier = $(new ELink,$1,$(gold_location))

# <QualifiedName> ::= Identifier '.' <QualifiedName>
$(gold_grammar)_produce-QualifiedName_Identifier_Dot         = $1.$3
# <QualifiedNameWithWildcard> ::= <QualifiedName> '.*'
$(gold_grammar)_produce-QualifiedNameWithWildcard_DotTimes   = $1.*


