#
#   Date: Sep 24, 2013
# Author: Eldar Abusalimov
#

# This Makefile expects the following variables to be externally provided:
#
#   TARGET: name of the output archive
#   A_FILES: input archives
#   O_FILES: input objects
#   RELOC:  a flag indicating whether to pass objects through LD or not
#   AR and ARFLAGS: archiver ('ar') and creation flags (e.g. 'rcs')
#   LD and LDFLAGS: linker (used when RELOC is set)
#

$(TARGET) :  # default

include mk/core/common.mk

fmt_line = $(addprefix \$(\n)$(\t)$(\t),$1)
normalize_path = $(patsubst $(abspath $(CURDIR))/%,%,$(abspath $1))


override O_FILES := $(sort $(call normalize_path,$(O_FILES)))
override A_FILES := $(sort $(call normalize_path,$(A_FILES)))


ifdef RELOC

define o_template
$(obj) : $o
endef

o_objs := \
	$(foreach o,$(O_FILES),$(foreach obj,$(dir $(TARGET))a.$(notdir $o), \
		$(eval $(value o_template))$(obj)))

ifneq ($(words $(O_FILES)),$(words $(sort $(notdir $(o_objs)))))
$(error Can't make an archive of objects with the same file name)
endif

else  # no RELOC
o_objs = $(O_FILES)
endif # RELOC


$(A_FILES:%=%.x) : %.x : %
	$(MKDIR) $@; cd $@; $(AR) x ../$(*F);


define a_template
members-$(abs) := $(shell $(AR) t '$a')  # x.o
$(members-$(abs):%=$a.x/%) : $a.x

ifdef RELOC
objects-$(abs) := $(members-$(abs):%=$a.%)  # path/to/lib.a.x.o
$(objects-$(abs)) : $a.% : $a.x/%
else  # no RELOC
objects-$(abs) := $(members-$(abs):%=$a.x/%)  # path/to/lib.a.x/x.o
endif # RELOC

endef

a_objs := $(foreach a,$(A_FILES),$(foreach abs,$(abspath $a), \
    $(eval $(value a_template))$(objects-$(abs))))


objs = $(o_objs) $(a_objs)

ifdef RELOC
$(objs) :
	$(LD) -r $(LDFLAGS) $< \
		-o $@
endif


$(TARGET) : $(objs)
	$(AR) $(ARFLAGS) $@ $(call fmt_line,$^)

