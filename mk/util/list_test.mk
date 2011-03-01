#
#   Date: Jan 21, 2011
# Author: Eldar Abusalimov
#

include util/math2.mk
include gmsl/gmsl.mk
include util/list.mk

# XXX got from gmsl-tests
.PHONY: all
all:
	@echo
	@echo Test Summary
	@echo ------------
	@echo "$(call int_decode,$(passed)) tests passed; $(call int_decode,$(failed)) tests failed"

passed :=
failed :=

ECHO := /bin/echo

start_test = $(shell $(ECHO) -n "Testing '$1': " >&2)$(eval current_test := OK)
stop_test  = $(shell $(ECHO) " $(subst $$,\$$,$(current_test))" >&2)
test_pass = .$(eval passed := $(call int_inc,$(passed)))
test_fail = X$(eval failed := $(call int_inc,$(failed)))$(eval current_test := ERROR '$$1' != '$$2')
test_assert = $(if $(filter undefined,$(origin 2)),$(eval 2 :=))$(shell \
  $(ECHO) -n $(if $(call seq,$1,$2),$(test_pass),$(test_fail)) >&2)

$(call start_test,list_scan)
$(call test_assert,$(call list_scan,divide,64,4 2 4),64 16 8 2)
$(call test_assert,$(call list_scan,divide,42,),42)
$(call test_assert,$(call list_scan,max,5,1 2 3 4),5 5 5 5 5)
$(call test_assert,$(call list_scan,max,5,1 2 3 4 5 6 7),5 5 5 5 5 5 6 7)
$(call stop_test)

$(call start_test,list_fold)
$(call test_assert,$(call list_fold,divide,64,4 2 4),2)
$(call test_assert,$(call list_fold,divide,42,),42)
$(call test_assert,$(call list_fold,max,5,1 2 3 4),5)
$(call test_assert,$(call list_fold,max,5,1 2 3 4 5 6 7),7)
$(call stop_test)

$(call start_test,list_reverse)
$(call test_assert,$(call list_reverse,1 2 3 4),4 3 2 1)
$(call test_assert,$(call list_reverse,1),1)
$(call test_assert,$(call list_reverse,),)
$(call test_assert,$(call list_reverse,  1 2   3 4 ),4 3 2 1)
$(call test_assert,$(call list_reverse, ),)
$(call stop_test)

$(call start_test,list_pairmap)
$(call test_assert,$(call list_pairmap,addsuffix,2 1 3,a b c),a2 b1 c3)
$(call test_assert,$(call list_pairmap,subst,q s d g,- - $$,asdfg),asdfg a-dfg as$$fg asdf)
$(call test_assert,$(call list_pairmap,subst,1 2,a b c,123abc),a23abc 1b3abc 123abcc)
$(call stop_test)

