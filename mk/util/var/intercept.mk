#
#   Date: Sep 10, 2010
# Author: Eldar Abusalimov
#

ifndef __util_check_mk
__util_check_mk := 1

include util/var/assign.mk

# 1. Target variable
# 2. Pre-expansion hook
var_intercept_attach_before = \
  $(__var_intercept_attach_check_init)$()

# 1. Target variable
# 2. Post-expansion hook
var_intercept_attach_after =

__var_intercept_attach_check_init = \
  $(if $(call var_defined))

check_attach = $(strip $(if $(value $(__unchecked_name)), \
  $(call var_assign_append,$(__check_hooks),$2), \
  $(__check_attach_init) \
)

__check_attach_init = \
  $(call var_assign_$(flavor $1),$(__unchecked_name),$(value $1)) \
  $(call var_assign_$(flavor $1),$1,$(__check_invoke)$$($(__unchecked_name))) \
  $(call var_assign_simple,$(__check_hooks),)

__check_invoke = $$(foreach check_target,$1 \
  ,$$(foreach __check_hook,$$($(__check_hooks)),$$(__check_hook)))

check_hook_assert_called = $(call assert_called,$(check_target),$0)

__unchecked_name = __unchecked_$1
__check_hooks = __checks_$1

# make FGRED=`echo -e '\e[31m'` FGRED1=`echo -e '\e[1;31m'` FGNORMAL=`echo -e '\e[0m'`
ifeq (1,0)
include util/var/assign.mk
__var_call_trace = \
  $(warning )$(and $(shell echo "$${FGRED1}$5$${FGNORMAL}" 1>&2),)$ \
  $(if $1,$(info 1.	$1))$ \
  $(if $2,$(info 2.	$2))$ \
  $(info )
__var_call_trace_result = \
  $(and $(shell echo "$${FGRED}$2" 1>&2),)$ \
  $(info $( )	$1)$ \
  $(and $(shell echo "$${FGNORMAL}" 1>&2),)$1$ \
  $(info )
__var_name_functions := \
  $(filter var_name_% __var_name_% var_list_% __var_list_%,$(.VARIABLES))
$(foreach var,$(__var_name_functions), \
  $(call var_assign_$(flavor $(var)),$(var),$ \
    $$(call __var_call_trace,$$1,$$2,$$3,,$(var))$ \
    $$(call __var_call_trace_result,$(value $(var)),$(var))) \
)
endif

endif # __util_check_mk
