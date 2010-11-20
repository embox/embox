#
# Make version utils.
#
# Author: Eldar Abusalimov
#

ifndef __util_make_version_mk
__util_make_version_mk := 1

include util/common.mk

# $(value 0),$(value 1) suppress possible undefined variables warning.
make_version_major = \
  $(call __make_version_part,make_version_major,$(value 0),$(value 1))
make_version_minor = \
  $(call __make_version_part,make_version_minor,$(value 0),$(value 1))

__make_version_part = $(strip \
  $(if $(call called,$1,$2), \
    $(or $(call __$1,$3),$(call assert,$(false),Invalid argument [$3])), \
    $(call __$1,$(MAKE_VERSION)) \
  ) \
)

__make_version = $(subst .,$(\space),$1)
__make_version_major = $(word 1,$(__make_version))
__make_version_minor = $(word 2,$(__make_version))

__make_version_cmp_strict = \
  $(call assert,$2,Must specify version to compare with)$ \
  $(call or,$ \
    $(call $1,$(make_version_major),$(call make_version_major,$2)),$ \
    $(call and,$ \
      $(call eq,$(make_version_major),$(call make_version_major,$2)),$ \
      $(call $1,$(make_version_minor),$(call make_version_minor,$2))$ \
    )$ \
  )

make_version_gt = $(call __gmsl_make_bool, \
  $(call assert_called,make_version_gt,$0) \
  $(call __make_version_cmp_strict,gt,$1) \
)
make_version_lt = $(call __gmsl_make_bool, \
  $(call assert_called,make_version_lt,$0) \
  $(call __make_version_cmp_strict,lt,$1) \
)
make_version_gte = $(call __gmsl_make_bool, \
  $(call assert_called,make_version_gte,$0) \
  $(call not,$(call make_version_lt,$1)) \
)
make_version_lte = $(call __gmsl_make_bool, \
  $(call assert_called,make_version_lte,$0) \
  $(call not,$(call make_version_gt,$1)) \
)
make_version_eq = $(call __gmsl_make_bool, \
  $(call assert_called,make_version_eq,$0) \
  $(call not,$(call or,$(call make_version_gt,$1)$(call make_version_lt,$1))) \
)
make_version_neq = $(call __gmsl_make_bool, \
  $(call assert_called,make_version_neq,$0) \
  $(call or,$(call make_version_gt,$1)$(call make_version_lt,$1)) \
)

endif # __util_make_version_mk
