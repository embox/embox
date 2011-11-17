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
define $(gold_grammar)_produce-%ID%
	$(gold_default_produce)# TODO Auto-generated stub!
endef

##END-RULES

endif # __gold_grammar_$(gold_grammar)_mk

