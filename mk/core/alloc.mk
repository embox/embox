#
#   Date: Feb 5, 2012
# Author: Eldar Abusalimov
#

ifndef __core_alloc_mk
__core_alloc_mk := 1

ALLOC_SCOPE ?=

alloc = \
	$(foreach cnt,$(words x \
		$(or $(value __alloc_cnt_$(ALLOC_SCOPE)-$1), \
			${eval __alloc_cnt_$(ALLOC_SCOPE)-$1 :=})) \
		,${eval __alloc_cnt_$(ALLOC_SCOPE)-$1 += $(cnt)}$(cnt)$(ALLOC_SCOPE))

alloc_last = \
	$(foreach cnt,$(lastword $(value __alloc_cnt_$(ALLOC_SCOPE)-$1)) \
		,$(cnt)$(ALLOC_SCOPE))

__cache_volatile += __alloc_cnt_%

endif # __core_alloc_mk
