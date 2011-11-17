##TEMPLATE-NAME 'Make - MyBuild engine (application stub)'
##LANGUAGE 'GNU Make'
##ENGINE-NAME 'MyBuild'
##AUTHOR 'Eldar Abusalimov'
##FILE-EXTENSION 'mk'
##NOTES
This template generates handler function stubs for symbols and rules.

Handlers are used to convert each node of a parse tree into
an application-specific representation.
##END-NOTES
##DELIMITER ','
##ID-SEPARATOR '_'
##PARAMETERS
# Application for '%Name%' grammar (version %Version%).
#    %About%
# Author: %Author%
##END-PARAMETERS

#
# All API functions and internals of the parser are defined in their own
# namespace which defaults to the basename of the definition script.
#
# This name is primarily used to refer the grammar when calling
# 'gold_parse_file' and 'gold_parse_stream' functions.
#

# Override this variable to define everything in a different namespace.
# Must be a single word.
gold_grammar := $(basename $(notdir $(lastword $(MAKEFILE_LIST))))

ifeq ($(call singleword,$(gold_grammar)),)
$(error 'gold_grammar' is empty or not a single word, \
	you have to define it properly in order to continue)
endif

gold_grammar := $(call trim,$(gold_grammar))

ifndef __gold_grammar_$(gold_grammar)_mk
__gold_grammar_$(gold_grammar)_mk := 1

# By default it is assumed that you'll place tables file in the same directory
# with this one and name it with '-tables' prefix.

# Fix the following line to override default behavior.
include $(dir $(lastword $(MAKEFILE_LIST)))$(gold_grammar)-tables.mk

#
# Symbols.
#
# Each symbol is converted by the corresponding constructor (if any has been
# defined). Constructor is a function with the following signature:
#
# Params:
#   1. For terminals: a list of decimal char codes representing the token.
#      For nonterminals: the result of production.
#   2. Start location in form 'line:column'.
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

##SYMBOLS
# Symbol: %Description%
#define $(gold_grammar)_create-%ID%
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

##END-SYMBOLS

#
# Stubs for contructors of constant terminals.
# Uncomment (and fix if needed) the lines corresponding to terminals which are
# known to be immutable (i.e. keywords and punctuation) eliminates the need to
# convert them from an input.
#

##SYMBOLS
#$(gold_grammar)_create-%ID.Padded% := %ID%
##END-SYMBOLS

#
# For each regular terminal we also define a constant with a human-readable
# description. Due to current limitations of template generator a definition
# is generated for all existing symbols, although variables for nonterminals
# are never actually used.
#

##SYMBOLS
$(gold_grammar)_name_of-%ID.Padded% := %Description%
##END-SYMBOLS


#
# Rules.
#
# As for symbols each rule can have a constructor that is used to produce an
# application-specific representation of the rule data.
# The signature of production function is the following:
#
# Params:
#  1..N: Each argument contains a value of the corresponding symbol
#        in the rule's RHS.
#  N+1:  Location vector with 'line:column' words each of which is a start of
#        the corresponding symbol.
#
# Return:
#   Converted value that is passed to a symbol handler corresponding to
#   the rule's LHS (if any has been defined).
#
# If production function is not defined then the rule is produced by
# concatenating the RHS through a space. To reuse this default value one can
# call 'gold_default_produce' function.
#

##RULES
# Rule: %Description%
# Args: 1..%SymbolCount% - Symbols; %SymbolCount%+1 - Location vector.
#define $(gold_grammar)_produce-%ID%
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

##END-RULES

$(def_all)

gold_grammar :=# Undefine.

endif # __gold_grammar_$(gold_grammar)_mk

