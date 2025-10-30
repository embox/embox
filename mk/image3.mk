#
#   Date: Mar 27, 2014
# Author: Anton Kozlov
#

embox_o   := $(OBJ_DIR)/embox-2.o
image_lds := $(OBJ_DIR)/mk/image.lds

.PHONY : all image FORCE
all : image
	@echo 'Build complete'

FORCE :

include $(ROOT_DIR)/mk/flags.mk
include $(ROOT_DIR)/mk/image_lib.mk

TARGET ?= embox$(if $(value PLATFORM),-$(PLATFORM))
TARGET := $(TARGET)$(if $(value LOCALVERSION),-$(LOCALVERSION))

IMAGE       = $(BIN_DIR)/$(TARGET)
IMAGE_DIS   = $(IMAGE).dis
IMAGE_BIN   = $(IMAGE).bin
IMAGE_SREC  = $(IMAGE).srec
IMAGE_SIZE  = $(IMAGE).size
IMAGE_PIGGY = $(IMAGE).piggy
IMAGE_A     = $(BIN_DIR)/lib$(TARGET).a
IMAGE_LINKED_A = $(BIN_DIR)/lib$(TARGET)-linked.a

ld_prerequisites =
ar_prerequisites =
initfs_prerequisites =
initfs_cp_prerequisites =

.SECONDEXPANSION:

include $(MKGEN_DIR)/include.mk
include $(__include_image)
include $(__include_initfs)
include $(__include)

cp_T_if_supported := \
	$(shell $(CP) --version 2>&1 | grep -l GNU >/dev/null && echo -T)

.SECONDARY:
.DELETE_ON_ERROR:

IMAGE_TARGET ?= executable
ifeq ($(IMAGE_TARGET),executable)
image: $(IMAGE)
image: $(IMAGE_BIN) $(IMAGE_SREC) $(IMAGE_SIZE)
ifeq ($(value DISASSEMBLY),y)
image : $(IMAGE_DIS)
endif
ifeq ($(value PIGGY),y)
image : $(IMAGE_PIGGY)
endif
else ifeq ($(IMAGE_TARGET),library)
image: $(IMAGE_A)
endif

ifndef LD_SINGLE_T_OPTION
ld_scripts_flag = $(1:%=-T%)
else
ld_scripts_flag = $(if $(strip $1),-T $1)
endif

LD_DISABLE_RELAXATION ?= n
ifeq (n,$(LD_DISABLE_RELAXATION))
relax = --relax
else
relax = --no-relax
endif

GEN_LINKER_MAP ?= n
ifeq ($(GEN_LINKER_MAP),y)
CC_MAP_FLAGS = -Wl,--cref -Wl,-Map,$@.map
LD_MAP_FLAGS = --cref -Map $@.map
else
CC_MAP_FLAGS :=
LD_MAP_FLAGS :=
endif

# This must be expanded in a secondary expansion context.
# NOTE: must be the last one in a list of prerequisites (contains order-only)
common_prereqs = $(ROOT_DIR)/mk/image2.mk $(ROOT_DIR)/mk/flags.mk $(MKGEN_DIR)/build.mk \
	$(if $(value mk_file),$(mk_file)) \
	| $(if $(value my_file),$(dir $(my_file:%=$(OBJ_DIR)/%)).) $(@D)/.

# Here goes image creation rules...

symbols_pass1_c = $(GEN_DIR)/symbols_pass1.c
symbols_pass2_c = $(GEN_DIR)/symbols_pass2.c

symbols_c_files = \
	$(symbols_pass1_c) \
	$(symbols_pass2_c)

$(symbols_pass1_c) : image_o = $(image_nosymbols_o)
$(symbols_pass2_c) : image_o = $(image_pass1_o)

SYMBOLS_WITH_FILENAME ?= 1

ifeq ($(SYMBOLS_WITH_FILENAME),1)
NM_OPTS := --demangle --line-numbers --numeric-sort
else
NM_OPTS := --demangle --numeric-sort
endif

$(symbols_c_files) :
$(symbols_c_files) : $(ROOT_DIR)/mk/script/nm2c.awk $$(common_prereqs)
$(symbols_c_files) : $$(image_o)
	$(NM) $(NM_OPTS) $< | $(AWK) -f $(ROOT_DIR)/mk/script/nm2c.awk > $@

symbols_pass1_a = $(OBJ_DIR)/symbols_pass1.a
symbols_pass2_a = $(OBJ_DIR)/symbols_pass2.a

symbols_a_files = \
	$(symbols_pass1_a) \
	$(symbols_pass2_a)

$(symbols_a_files) : %.a : %.o
	$(AR) $(ARFLAGS) $@ $<

$(symbols_a_files:%.a=%.o) : flags_before :=
$(symbols_a_files:%.a=%.o) : flags :=

# workaround to get VPATH and GPATH to work with an OBJ_DIR.
#$(shell $(MKDIR) $(OBJ_DIR) 2> /dev/null)
#GPATH := $(OBJ_DIR:$(ROOT_DIR)/%=%) $(GEN_DIR:$(ROOT_DIR)/%=%)
#VPATH += $(GPATH)

image_relocatable_o = $(OBJ_DIR)/image_relocatable.o
image_nosymbols_o = $(OBJ_DIR)/image_nosymbols.o
image_pass1_o = $(OBJ_DIR)/image_pass1.o

image_files := $(IMAGE) $(image_nosymbols_o) $(image_pass1_o)

$(IMAGE_A) : $(embox_o) $$(common_prereqs)
	@$(RM) $@
	$(AR) rcs $@ $<

$(IMAGE_LINKED_A) : $(image_relocatable_o) $$(common_prereqs)
	@$(RM) $@
	$(AR) rcs $@ $<

#XXX
FINAL_LINK_WITH_CC ?=
ifeq (1,$(FINAL_LINK_WITH_CC))

ram_sz :=$(shell echo LDS_REGION_SIZE_RAM | $(GCC) -E -P -imacros $(SRCGEN_DIR)/config.lds.h - | sed 's/M/*1024*1024/' | bc)
phymem_cflags_addon := \
	-Wl,--defsym=_ram_base=__phymem_space \
	-Wl,--defsym=_reserve_end=__phymem_space \
	-Wl,--defsym=_ram_size=$(ram_sz) \
	-DPHYMEM_SPACE_SIZE=$(ram_sz) \
	mk/phymem_cc_addon.tmpl.c

FINAL_LDFLAGS ?=
$(image_relocatable_o): $(image_lds) $(embox_o) $$(common_prereqs)
	$(GCC) -Wl,--relocatable \
	$(embox_o) \
	$(FINAL_LDFLAGS) \
	-Wl,--defsym=__symbol_table=0 \
	-Wl,--defsym=__symbol_table_size=0 \
	$(phymem_cflags_addon) \
	$(CC_MAP_FLAGS) \
	-o $@

$(image_nosymbols_o): $(image_lds) $(embox_o) $$(common_prereqs)
	$(GCC) -Wl,$(relax) \
	$(embox_o) \
	$(FINAL_LDFLAGS) \
	-Wl,--defsym=__symbol_table=0 \
	-Wl,--defsym=__symbol_table_size=0 \
	$(phymem_cflags_addon) \
	$(CC_MAP_FLAGS) \
	-o $@

$(image_pass1_o): $(image_lds) $(embox_o) $(symbols_pass1_a) $$(common_prereqs)
	$(GCC) -Wl,$(relax) \
	$(embox_o) \
	$(FINAL_LDFLAGS) \
	$(symbols_pass1_a) \
	$(phymem_cflags_addon) \
	$(CC_MAP_FLAGS) \
	-o $@

$(IMAGE): $(image_lds) $(embox_o) $(symbols_pass2_a) $$(common_prereqs)
	$(GCC) -Wl,$(relax) \
	$(embox_o) \
	$(FINAL_LDFLAGS) \
	$(symbols_pass2_a) \
	$(phymem_cflags_addon) \
	$(CC_MAP_FLAGS) \
	-o $@ | tee $@.mem

else # FINAL_LINK_WITH_CC

$(image_relocatable_o): $(image_lds) $(embox_o) $$(common_prereqs)
	$(LD) --relocatable $(ldflags) \
	-T $(image_lds) \
	$(embox_o) \
	--defsym=__symbol_table=0 \
	--defsym=__symbol_table_size=0 \
	$(LD_MAP_FLAGS) \
	-o $@

$(image_nosymbols_o): $(image_lds) $(embox_o) $$(common_prereqs)
	$(LD) $(relax) $(ldflags) \
	--gc-sections \
	-T $(image_lds) \
	$(embox_o) \
	--defsym=__symbol_table=0 \
	--defsym=__symbol_table_size=0 \
	$(LD_MAP_FLAGS) \
	-o $@

$(image_pass1_o): $(image_lds) $(embox_o) $(symbols_pass1_a) $$(common_prereqs)
	$(LD) $(relax) $(ldflags) \
	--gc-sections \
	-T $(image_lds) \
	$(embox_o) \
	$(symbols_pass1_a) \
	$(LD_MAP_FLAGS) \
	-o $@

gensums_py := $(ROOT_DIR)/mk/gensums.py
md5sums1_o := $(OBJ_DIR)/md5sums1.o
md5sums2_o := $(OBJ_DIR)/md5sums2.o
image_nocksum := $(OBJ_DIR)/image_nocksum.o

$(GEN_DIR)/md5sums1.c : source = $(image_pass1_o)
$(GEN_DIR)/md5sums2.c : source = $(image_nocksum)

$(GEN_DIR)/md5sums1.c $(GEN_DIR)/md5sums2.c: $$(source)
	echo "/* Generated md5sums */" > $@
	for sect in text rodata; do \
		$(OBJCOPY) -j .$$sect -O binary $< $@.$$sect.bin ; \
		$(NM) $< | $(gensums_py) $$sect $@.$$sect.bin 0x$$($(OBJDUMP) -h $< | grep .$$sect | sed -E "s/ +/ /g" | cut -d " " -f 5) >> $@ ; \
	done

$(md5sums1_o) : $(GEN_DIR)/md5sums1.c
$(md5sums2_o) : $(GEN_DIR)/md5sums2.c

$(image_nocksum): $(image_lds) $(embox_o) $(md5sums1_o) $(symbols_pass2_a) $$(common_prereqs)
	$(LD) $(relax) $(ldflags) \
	--gc-sections \
	-T $(image_lds) \
	$(embox_o) \
	$(md5sums1_o) \
	$(symbols_pass2_a) \
	$(LD_MAP_FLAGS) \
	-o $@

$(IMAGE): $(image_lds) $(embox_o) $(md5sums2_o) $(symbols_pass2_a) $$(common_prereqs)
	$(LD) $(relax) $(ldflags) \
	--gc-sections \
	-T $(image_lds) \
	$(embox_o) \
	$(md5sums2_o) \
	$(symbols_pass2_a) \
	$(LD_MAP_FLAGS) \
	--print-memory-usage \
	-o $@ | tee $@.mem

endif # FINAL_LINK_WITH_CC

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

$(IMAGE_SIZE): $(IMAGE)
	@if which $(SIZE) >/dev/null 2>&1; then \
	    $(SIZE) $^ | tee $@; \
	else \
		echo "$(SIZE) util not found" > $@; \
	fi

ifdef ROOTFS_OUT_DIR

.PHONY : __copy_user_rootfs
all : __copy_user_rootfs $(__cpio_files)

$(__cpio_files) : FORCE
	$(foreach f,$(patsubst $(abspath $(ROOTFS_DIR))/%,$(ROOTFS_OUT_DIR)/%,$(abspath $@)), \
		$(CP) -r $(cp_T_if_supported) $(src_file) $f; \
		$(foreach c,chmod chown,$(if $(and $($c),$(findstring $($c),'')),,$c $($c) $f;)) \
		$(foreach a,$(strip $(subst ',,$(xattr))), \
			attr -s $(basename $(subst =,.,$a)) -V $(subst .,,$(suffix $(subst =,.,$a))) $f;) \
		find $f -name .gitkeep -type f -print0 | xargs -0 rm -rf)

__copy_user_rootfs :
	if [ -d $(USER_ROOTFS_DIR) ]; \
	then \
		cd $(USER_ROOTFS_DIR) \
			&& cp -r * $(abspath $(ROOTFS_OUT_DIR)); \
	fi
endif
