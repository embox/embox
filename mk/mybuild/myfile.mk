# Application for 'MyFile' grammar (version 0.1).
#    MyBuild file
# Author: Eldar Abusalimov

#
# All API functions and internals of the parser are defined in their own
# namespace which defaults to the basename of the definition script.
#
# This name is primarily used to refer the grammar when calling 'gold_parse'.
#

gold_grammar := $(basename $(notdir $(lastword $(MAKEFILE_LIST))))

ifeq ($(call singleword,$(gold_grammar)),)
$(error 'gold_grammar' is empty or not a single word, \
	you have to define it properly in order to continue)
endif

gold_grammar := $(call trim,$(gold_grammar))

ifndef __gold_grammar_$(gold_grammar)_mk
__gold_grammar_$(gold_grammar)_mk := 1

include $(addprefix $(dir $(lastword $(MAKEFILE_LIST)))$(gold_grammar)-, \
			tables.mk symbols.mk rules.mk)

$(def_all)

gold_grammar :=# Undefine.

endif # __gold_grammar_$(gold_grammar)_mk

