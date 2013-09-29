#
#   Date: Sep 24, 2013
# Author: Eldar Abusalimov
#

# This Makefile expects the following variables to be externally provided:
#
#   TARGET: name of the output archive
#   A_FILES: input archives
#   O_FILES: input objects
#   APP_ID:  an identifier which (if set) is used to rename object sections
#   AR and ARFLAGS: archiver ('ar') and creation flags (e.g. 'rcs')
#   OBJCOPY and OBJCOPYFLAGS: used when APP_ID is set
#

$(TARGET) :  # default

include mk/core/common.mk

fmt_line = $(addprefix \$(\n)$(\t)$(\t),$1)
normalize_path = $(patsubst $(abspath $(CURDIR))/%,%,$(abspath $1))


override O_FILES := $(sort $(call normalize_path,$(O_FILES)))
override A_FILES := $(sort $(call normalize_path,$(A_FILES)))


ifdef APP_ID

define o_template
$(obj) : $o
endef

o_objs := \
	$(foreach o,$(O_FILES),$(foreach obj,$(dir $(TARGET))a.$(notdir $o), \
		$(eval $(value o_template))$(obj)))

ifneq ($(words $(O_FILES)),$(words $(sort $(notdir $(o_objs)))))
$(error Can't make an archive of objects with the same file name)
endif

else  # no APP_ID
o_objs = $(O_FILES)
endif # APP_ID


$(A_FILES:%=%.x) : %.x : %
	$(MKDIR) $@; cd $@; $(AR) x ../$(*F);


define a_template
members-$(abs) := $(shell $(AR) t '$a')  # x.o
$(members-$(abs):%=$a.x/%) : $a.x

ifdef APP_ID
objects-$(abs) := $(members-$(abs):%=$a.%)  # path/to/lib.a.x.o
$(objects-$(abs)) : $a.% : $a.x/%
else  # no APP_ID
objects-$(abs) := $(members-$(abs):%=$a.x/%)  # path/to/lib.a.x/x.o
endif # APP_ID

endef

a_objs := $(foreach a,$(A_FILES),$(foreach abs,$(abspath $a), \
    $(eval $(value a_template))$(objects-$(abs))))


objs = $(o_objs) $(a_objs)

ifdef APP_ID
$(objs) :
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@
endif


$(TARGET) : $(objs)
	$(AR) $(ARFLAGS) $@ $(call fmt_line,$^)

