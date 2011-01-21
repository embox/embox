#
#   Date: Jan 21, 2011
# Author: Eldar Abusalimov
#

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
stop_test  = $(shell $(ECHO) " $(current_test)" >&2)
test_pass = .$(eval passed := $(call int_inc,$(passed)))
test_fail = X$(eval failed := $(call int_inc,$(failed)))$(eval current_test := ERROR '$1' != '$2')
test_assert = $(if $(filter undefined,$(origin 2)),$(eval 2 :=))$(shell $(ECHO) -n $(if $(call seq,$1,$2),$(call test_pass,$1,$2),$(call test_fail,$1,$2)) >&2)

$(call start_test,list_scanl)
$(call test_assert,$(call list_scanl,divide,64,4 2 4),64 16 8 2)
$(call test_assert,$(call list_scanl,divide,42,),42)
$(call test_assert,$(call list_scanl,max,5,1 2 3 4),5 5 5 5 5)
$(call test_assert,$(call list_scanl,max,5,1 2 3 4 5 6 7),5 5 5 5 5 5 6 7)
$(call stop_test)

$(call start_test,list_foldl)
$(call test_assert,$(call list_foldl,divide,64,4 2 4),2)
$(call test_assert,$(call list_foldl,divide,42,),42)
$(call test_assert,$(call list_foldl,max,5,1 2 3 4),5)
$(call test_assert,$(call list_foldl,max,5,1 2 3 4 5 6 7),7)
$(call stop_test)

$(call start_test,list_foldl1)
$(call test_assert,$(call list_foldl1,plus,1 2 3 4),10)
$(call test_assert,$(call list_foldl1,divide,64 4 2 8),1)
$(call test_assert,$(call list_foldl1,divide,42,),42)
$(call test_assert,$(call list_foldl1,max,1 2 3 4 5 6 7),7)
$(call test_assert,$(call list_foldl1,max,3 6 12 4 55 11),55)
$(call stop_test)
