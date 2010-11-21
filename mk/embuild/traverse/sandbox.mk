#
# Well, it is not quite a sandbox, as one might think. It's just a kind of
# wrapper around some code which filters all newly defined variables.
#
#   Date: Nov 19, 2010
# Author: Eldar Abusalimov
#

# It is assumed that sandbox will be invoked only once.
ifdef __sandbox_variables_before
$(error Sandbox must not be included more than one time)
endif # __sandbox_variables_before
__sandbox_variables_before :=

#
# Sandbox interprets sandbox_action as 'make' code, grabs all new variables
# defined by that code and calls sandbox_do_process_variable for each variable.
# The result of that foreach iteration can then be found in sandbox_result.
#
# Please note that sandbox does not track changes of already existing
# variables. Only newly defined ones are processed.
#
ifndef sandbox_action
$(error Must specify action to perform)
endif # sandbox_action

ifndef sandbox_do_process_variable
$(error Must specify callback function for variable processing)
endif # sandbox_do_process_variable

__sandbox_variables_before := $(.VARIABLES)

# Go!
${eval $(sandbox_action)}

__sandbox_variables_after  := $(.VARIABLES)

include util/var_name.mk

sandbox_result := $(foreach __sandbox_var,$(filter-out \
   $(call var_name_escape,$(__sandbox_variables_before)), \
   $(call var_name_escape,$(__sandbox_variables_after)) \
       ),$(call sandbox_do_process_variable \
               ,$(call var_name_unescape,$(__sandbox_var))))
