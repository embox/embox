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
# This name is primarily used to refer the grammar when calling 'gold_parse'.
#

# TODO Override this variable to define everything in a different namespace.
# Must be a single word.
gold_grammar := $(basename $(notdir $(lastword $(MAKEFILE_LIST))))

ifeq ($(call singleword,$(gold_grammar)),)
$(error 'gold_grammar' is empty or not a single word, \
	you have to define it properly in order to continue)
endif

gold_grammar := $(call trim,$(gold_grammar))

ifndef __gold_grammar_$(gold_grammar)_mk
__gold_grammar_$(gold_grammar)_mk := 1

# TODO By default it is assumed that you'll place grammar-related files
# in the same directory with this one and name them
# with '-tables', '-symbols' and '-rules' prefixes.
# Fix the following 'include' directive to override the default behavior.
include $(addprefix $(dir $(lastword $(MAKEFILE_LIST)))$(gold_grammar)-, \
			tables.mk symbols.mk rules.mk)

$(def_all)

gold_grammar :=# Undefine.

endif # __gold_grammar_$(gold_grammar)_mk

