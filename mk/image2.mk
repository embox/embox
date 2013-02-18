#
#   Date: Jun 22, 2012
# Author: Eldar Abusalimov
#

.PHONY : all image
all : image
	@echo 'Build complete'

# Run external builders prior to anything else.
-include __extbld
.PHONY : __extbld
__extbld :

include mk/core/common.mk

include $(MKGEN_DIR)/build.mk

TARGET ?= embox$(if $(value PLATFORM),-$(PLATFORM))
TARGET := $(TARGET)$(if $(value LOCALVERSION),-$(LOCALVERSION))

IMAGE       = $(BIN_DIR)/$(TARGET)
IMAGE_DIS   = $(IMAGE).dis
IMAGE_BIN   = $(IMAGE).bin
IMAGE_SREC  = $(IMAGE).srec
IMAGE_SIZE  = $(IMAGE).size
IMAGE_PIGGY = $(IMAGE).piggy

include mk/flags.mk # It must be included after a user-defined config.

.SECONDEXPANSION:
include $(MKGEN_DIR)/include.mk

.SECONDARY:
.DELETE_ON_ERROR:

image: $(IMAGE)
image: $(IMAGE_DIS) $(IMAGE_BIN) $(IMAGE_SREC) $(IMAGE_SIZE) $(IMAGE_PIGGY)

CROSS_COMPILE ?=

CC      := $(CROSS_COMPILE)gcc
CPP     := $(CC) -E
CXX     := $(CROSS_COMPILE)g++
AR      := $(CROSS_COMPILE)ar
AS      := $(CROSS_COMPILE)as
LD      := $(CROSS_COMPILE)ld
NM      := $(CROSS_COMPILE)nm
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size

# This must be expanded in a secondary expansion context.
common_prereqs_nomk  = mk/image2.mk mk/flags.mk $(MKGEN_DIR)/build.mk
common_prereqs       = $(common_prereqs_nomk) $(mk_file)
extbld_prerequisites = $(common_prereqs)

VPATH := $(SRCGEN_DIR)

%/. :
	@$(MKDIR) $*

cc_prerequisites    = $(common_prereqs) $(extra_prereqs)

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.c | $$(@D)/.
	$(CC) $(CFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.S | $$(@D)/.
	$(CC) $(ASFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.cpp | $$(@D)/.
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<
$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.cxx | $$(@D)/.
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

cpp_prerequisites   = $(common_prereqs) $(extra_prereqs)
$(OBJ_DIR)/%.lds : $(ROOT_DIR)/%.lds.S | $$(@D)/.
	$(CPP) -P -undef $(CPPFLAGS) $(flags) -imacros $(SRCGEN_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -o $@ $<

initfs_cp_prerequisites = $(common_prereqs) $(src_file) $(extra_prereqs)
$(ROOTFS_DIR)/% : | $(ROOTFS_DIR)/.
	@$(CP) -r -T $(src_file) $@
	@find $@ -name .gitkeep -type f -print0 | xargs -0 /bin/rm -rf
$(ROOTFS_DIR)/. :
	@$(MKDIR) $(@D)

fmt_line = $(addprefix \$(\n)$(\t)$(\t),$1)

initfs_prerequisites = $(common_prereqs) $(cpio_files) \
	$(wildcard $(USER_ROOTFS_DIR) $(USER_ROOTFS_DIR)/*)
$(ROOTFS_IMAGE) : rel_cpio_files = \
		$(patsubst $(abspath $(ROOTFS_DIR))/%,%,$(abspath $(cpio_files)))
$(ROOTFS_IMAGE) : | $$(@D)/. $(ROOTFS_DIR)/.
$(ROOTFS_IMAGE) :
	cd $(ROOTFS_DIR) \
		&& find $(rel_cpio_files) -depth -print | cpio -L --quiet -H newc -o -O $(abspath $@)
	if [ -d $(USER_ROOTFS_DIR) ]; \
	then \
		cd $(USER_ROOTFS_DIR) \
			&& find * -depth -print | cpio -L --quiet -H newc -o --append -O $(abspath $@); \
	fi
	@FILES=`find $(cpio_files) $(USER_ROOTFS_DIR)/* -depth -print 2>/dev/null`; \
	{                                            \
		printf '$(ROOTFS_IMAGE):';               \
		for dep in $$FILES;                      \
			do printf ' \\\n\t%s' "$$dep"; done; \
		printf '\n';                             \
		for dep in $$FILES;                      \
			do printf '\n%s:\n' "$$dep"; done;   \
	} > $@.d
-include $(ROOTFS_IMAGE).d

#XXX
$(OBJ_DIR)/src/fs/driver/initfs/initfs_cpio.o : $(ROOTFS_IMAGE)

ar_prerequisites    = $(common_prereqs) $(ar_objs)
$(OBJ_DIR)/%.a : | $$(@D)/.
	$(AR) $(ARFLAGS) $@ $(call fmt_line,$(ar_objs))

# Here goes image creation rules...

symbols_pass1_c = $(OBJ_DIR)/symbols_pass1.c
symbols_pass2_c = $(OBJ_DIR)/symbols_pass2.c

symbols_c_files = \
	$(symbols_pass1_c) \
	$(symbols_pass2_c)

$(symbols_pass1_c) : image_o = $(image_nosymbols_o)
$(symbols_pass2_c) : image_o = $(image_pass1_o)

$(symbols_c_files) :
$(symbols_c_files) : $$(common_prereqs_nomk) mk/script/nm2c.awk | $$(@D)/.
$(symbols_c_files) : $$(image_o)
	$(NM) --demangle -n $< | awk -f mk/script/nm2c.awk > $@

symbols_pass1_a = $(OBJ_DIR)/symbols_pass1.a
symbols_pass2_a = $(OBJ_DIR)/symbols_pass2.a

symbols_a_files = \
	$(symbols_pass1_a) \
	$(symbols_pass2_a)

$(symbols_a_files) : %.a : %.o
	$(AR) $(ARFLAGS) $@ $<

$(symbols_a_files:%.a=%.o) : flags :=

# workaround to get VPATH and GPATH to work with an OBJ_DIR.
$(shell $(MKDIR) $(OBJ_DIR) 2> /dev/null)
GPATH := $(OBJ_DIR:$(ROOT_DIR)/%=%)
VPATH += $(GPATH)

image_nosymbols_o = $(OBJ_DIR)/image_nosymbols.o
image_pass1_o = $(OBJ_DIR)/image_pass1.o

image_files := $(IMAGE) $(image_nosymbols_o) $(image_pass1_o)

__define_image_rules = $(eval $(value __image_rule))
$(call __define_image_rules,$(image_files))

image_prerequisites = $(mk_file) \
	$(ld_scripts) $(ld_objs) $(ld_libs)
ifndef LD_SINGLE_T_OPTION
$(image_files): ld_scripts_flag = $(ld_scripts:%=-T%)
else
$(image_files): ld_scripts_flag = -T $(ld_scripts)
endif
$(image_files): ldflags_all = $(LDFLAGS) $(call fmt_line,$(ld_scripts_flag))

$(image_nosymbols_o): | $$(@D)/. $(dir $(IMAGE).map).
	$(LD) --relocatable $(ldflags) \
	$(call fmt_line,$(ld_objs)) \
	$(call fmt_line,$(ld_libs)) \
	--cref -Map $(IMAGE)_nosymbols.map \
	-o $@

$(image_pass1_o) : $(image_nosymbols_o) $(symbols_pass1_a) | $$(@D)/.
	$(LD) --relax $(ldflags_all) \
		$(image_nosymbols_o) \
		$(symbols_pass1_a) \
		$(call fmt_line,$(ld_libs)) \
		--cref -Map $(IMAGE)_pass1.map \
	-o $@

$(IMAGE): $(image_nosymbols_o) $(symbols_pass2_a) | $$(@D)/.
	$(LD) --relax $(ldflags_all) \
		$(image_nosymbols_o) \
		$(symbols_pass2_a) \
		$(call fmt_line,$(ld_libs)) \
		--cref -Map $(IMAGE).map \
	-o $@

$(IMAGE_DIS): $(IMAGE)
	$(OBJDUMP) -S $< > $@

$(IMAGE_SREC): $(IMAGE)
	@$(OBJCOPY) -O srec $< $@

$(IMAGE_BIN): $(IMAGE)
	@$(OBJCOPY) -O binary $< $@

$(IMAGE_PIGGY): $(IMAGE)
	@$(OBJCOPY) -O binary -R .note -R .comment -S $< $@.tmp
	@$(LD) -r -b binary $@.tmp -o $@
	@$(RM) $@.tmp

image_size_sort = \
	echo "" >> $@;                    \
	echo "sort by $2 size" >> $@;     \
	cat $@.tmp | sort -g -k $1 >> $@;

$(IMAGE_SIZE): $(IMAGE)
	@if [ `which $(SIZE) 2> /dev/null` ];  \
	then                                   \
	    $(SIZE) $^ > $@.tmp;               \
		echo "size util generated output for $(TARGET)" > $@; \
		$(call image_size_sort,1,text)     \
		$(call image_size_sort,2,data)     \
		$(call image_size_sort,3,bss)      \
		$(call image_size_sort,4,total)    \
		$(RM) $@.tmp;                      \
	else                                   \
		echo "$(SIZE) util not found" > $@;   \
	fi;

