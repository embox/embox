#
#   Date: Sep 10, 2010
# Author: Eldar Abusalimov
#

ifndef __util_check_mk
__util_check_mk := 1

include util/var_assign.mk

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

endif # __util_check_mk
