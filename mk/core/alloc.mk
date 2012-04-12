#
#   Date: Feb 5, 2012
# Author: Eldar Abusalimov
#

ifndef __core_alloc_mk
__core_alloc_mk := 1

ALLOC_SCOPE ?=

alloc = \
	$(call __alloc,__alloc_cnt_$(ALLOC_SCOPE)-$1)$(ALLOC_SCOPE)
__alloc = \
	${eval $1 := $(call inc,$(value $1))}$(subst .,,$($1))

alloc_last = \
	$(subst .,,$(value __alloc_cnt_$(ALLOC_SCOPE)-$1))$(ALLOC_SCOPE)

__cache_volatile += __alloc_cnt_%

# Increments the given numeber.
#   1. Decimal with periods between digits: 42='.4.2'
inc = \
	$(call __inc$(suffix .0$1),$(basename $1))

__inc.0 = $1.1
__inc.1 = $1.2
__inc.2 = $1.3
__inc.3 = $1.4
__inc.4 = $1.5
__inc.5 = $1.6
__inc.6 = $1.7
__inc.7 = $1.8
__inc.8 = $1.9
__inc.9 = $(call inc,$1).0

to_dec = $(subst .,,$1)

endif # __core_alloc_mk
