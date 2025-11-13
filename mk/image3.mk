#
#   Date: Mar 27, 2014
# Author: Anton Kozlov
#

embox_o   := $(OBJ_DIR)/embox-2.o
image_lds := $(OBJ_DIR)/image.lds

.PHONY : all image FORCE
all : image
	@echo 'Build complete'

FORCE :

include $(ROOT_DIR)/mk/flags.mk
include $(ROOT_DIR)/mk/image_lib.mk

TARGET := $(TARGET)$(if $(value LOCALVERSION),-$(LOCALVERSION))

IMAGE          := $(BIN_DIR)/$(TARGET)
IMAGE_DIS      := $(IMAGE).dis
IMAGE_BIN      := $(IMAGE).bin
IMAGE_SREC     := $(IMAGE).srec
IMAGE_SIZE     := $(IMAGE).size
IMAGE_PIGGY    := $(IMAGE).piggy
IMAGE_A        := $(BIN_DIR)/lib$(TARGET).a
IMAGE_LINKED_A := $(BIN_DIR)/lib$(TARGET)-linked.a

md5sums1_o          := $(OBJ_DIR)/md5sums1.o
md5sums2_o          := $(OBJ_DIR)/md5sums2.o
symbols_pass1_a     := $(OBJ_DIR)/symbols_pass1.a
symbols_pass2_a     := $(OBJ_DIR)/symbols_pass2.a
image_relocatable_o := $(OBJ_DIR)/image_relocatable.o
image_nosymbols_o   := $(OBJ_DIR)/image_nosymbols.o
image_pass1_o       := $(OBJ_DIR)/image_pass1.o
image_nocksum       := $(OBJ_DIR)/image_nocksum.o

ld_prerequisites =
ar_prerequisites =
initfs_prerequisites =
initfs_cp_prerequisites =

.SECONDEXPANSION :

include $(MKGEN_DIR)/include.mk
include $(__include_image)
include $(__include_initfs)
include $(__include)

cp_T_if_supported := \
	$(shell $(CP) --version 2>&1 | grep -l GNU >/dev/null && echo -T)

.SECONDARY :
.DELETE_ON_ERROR :

IMAGE_TARGET ?= executable
ifeq ($(IMAGE_TARGET),executable)
image : $(IMAGE)
image : $(IMAGE_BIN) $(IMAGE_SREC) $(IMAGE_SIZE)
ifeq ($(value DISASSEMBLY),y)
image : $(IMAGE_DIS)
endif
ifeq ($(value PIGGY),y)
image : $(IMAGE_PIGGY)
endif
else ifeq ($(IMAGE_TARGET),library)
image : $(IMAGE_A)
endif

image_files :=
image_has_symbol = $(strip $(shell $(NM) -g $(embox_o) \
	| grep "$(1)" >/dev/null && echo y || echo n))

LD_DISABLE_RELAXATION ?= n
ifeq (n,$(LD_DISABLE_RELAXATION))
relax := --relax
else
relax := --no-relax
endif

SYMBOLS_WITH_FILENAME ?= y
ifeq ($(SYMBOLS_WITH_FILENAME),y)
nm_opts := --demangle --line-numbers --numeric-sort
else
nm_opts := --demangle --numeric-sort
endif

GEN_MODULE_MD5SUM ?= n
ifeq ($(GEN_MODULE_MD5SUM),y)
image_files += $(md5sums2_o)
endif

ifeq ($(call image_has_symbol,__symbol_table_size),y)
image_files += $(symbols_pass2_a)
endif

# This must be expanded in a secondary expansion context.
# NOTE: must be the last one in a list of prerequisites (contains order-only)
common_prereqs = $(ROOT_DIR)/mk/image2.mk $(ROOT_DIR)/mk/flags.mk $(MKGEN_DIR)/build.mk \
	$(if $(value mk_file),$(mk_file)) \
	| $(if $(value my_file),$(dir $(my_file:%=$(OBJ_DIR)/%)).) $(@D)/.

symbols_pass1_c = $(GEN_DIR)/symbols_pass1.c
symbols_pass2_c = $(GEN_DIR)/symbols_pass2.c

symbols_c_files = \
	$(symbols_pass1_c) \
	$(symbols_pass2_c)

$(symbols_pass1_c) : image_o = $(image_nosymbols_o)
$(symbols_pass2_c) : image_o = $(image_pass1_o)

$(symbols_c_files) :
$(symbols_c_files) : $(ROOT_DIR)/mk/script/nm2c.awk $$(common_prereqs)
$(symbols_c_files) : $$(image_o)
	$(NM) $(nm_opts) $< | $(AWK) -f $(ROOT_DIR)/mk/script/nm2c.awk > $@

symbols_a_files = \
	$(symbols_pass1_a) \
	$(symbols_pass2_a)

$(symbols_a_files) : %.a : %.o
	$(AR) $(ARFLAGS) $@ $<

$(symbols_a_files:%.a=%.o) : flags_before :=
$(symbols_a_files:%.a=%.o) : flags :=

$(image_relocatable_o) : $(image_lds) $(embox_o) $$(common_prereqs)
	$(LD) --relocatable $(ldflags) \
	-T $(image_lds) \
	$(embox_o) \
	--defsym=__symbol_table=0 \
	--defsym=__symbol_table_size=0 \
	-o $@

$(image_nosymbols_o) : $(image_lds) $(embox_o) $$(common_prereqs)
	$(LD) $(relax) $(ldflags) \
	--gc-sections \
	-T $(image_lds) \
	$(embox_o) \
	--defsym=__symbol_table=0 \
	--defsym=__symbol_table_size=0 \
	-o $@

$(image_pass1_o) : $(image_lds) $(embox_o) $(symbols_pass1_a) $$(common_prereqs)
	$(LD) $(relax) $(ldflags) \
	--gc-sections \
	-T $(image_lds) \
	$(embox_o) \
	$(symbols_pass1_a) \
	-o $@

gensums_py := $(ROOT_DIR)/mk/gensums.py

$(GEN_DIR)/md5sums1.c : source = $(image_pass1_o)
$(GEN_DIR)/md5sums2.c : source = $(image_nocksum)

$(GEN_DIR)/md5sums1.c $(GEN_DIR)/md5sums2.c : $$(source)
	echo "/* Generated md5sums */" > $@
	for sect in text rodata; do \
		$(OBJCOPY) -j .$$sect -O binary $< $@.$$sect.bin ; \
		$(NM) $< | $(gensums_py) $$sect $@.$$sect.bin 0x$$($(OBJDUMP) -h $< \
			| grep .$$sect | sed -E "s/ +/ /g" | cut -d " " -f 5) >> $@ ; \
	done

$(md5sums1_o) : $(GEN_DIR)/md5sums1.c
$(md5sums2_o) : $(GEN_DIR)/md5sums2.c

$(image_nocksum) : $(image_lds) $(embox_o) $(md5sums1_o) $$(common_prereqs)
	$(LD) $(relax) $(ldflags) \
	--gc-sections \
	-T $(image_lds) \
	$(embox_o) \
	$(md5sums1_o) \
	--defsym=__symbol_table=0 \
	--defsym=__symbol_table_size=0 \
	-o $@

ifneq ($(filter usermode%,$(ARCH)),)
ram_sz := $(shell echo LDS_REGION_SIZE_RAM \
	| $(GCC) -E -P -imacros $(SRCGEN_DIR)/config.lds.h - | sed 's/M/*1024*1024/' | bc)
$(IMAGE) : $(embox_o) $$(common_prereqs)
	$(GCC) -Wl,$(relax) \
	$(embox_o) \
	$(CFLAGS) \
	-Wl,--defsym=__symbol_table=0 \
	-Wl,--defsym=__symbol_table_size=0 \
	-Wl,--defsym=_ram_base=__phymem_space \
	-Wl,--defsym=_reserve_end=__phymem_space \
	-Wl,--defsym=_ram_size=$(ram_sz) \
	-DPHYMEM_SPACE_SIZE=$(ram_sz) \
	mk/phymem_cc_addon.tmpl.c \
	-o $@
else
$(IMAGE) : $(image_lds) $(embox_o) $(image_files) $$(common_prereqs)
	$(LD) $(relax) $(ldflags) \
	--gc-sections \
	-T $(image_lds) \
	$(embox_o) \
	$(image_files) \
	--print-memory-usage \
	-o $@ | tee $@.mem
endif

$(IMAGE_DIS) : $(IMAGE)
	$(OBJDUMP) -S $< > $@

$(IMAGE_BIN) : $(IMAGE)
	@$(OBJCOPY) -O binary $< $@

$(IMAGE_SREC) : $(IMAGE)
	@$(OBJCOPY) -O srec $< $@

$(IMAGE_PIGGY) : $(IMAGE)
	@$(OBJCOPY) -O binary -R .note -R .comment -S $< $@.tmp
	@$(LD) -r -b binary $@.tmp -o $@
	@$(RM) $@.tmp

$(IMAGE_SIZE) : $(IMAGE)
	@if which $(SIZE) >/dev/null 2>&1; then \
	    $(SIZE) $^ | tee $@; \
	else \
		echo "$(SIZE) util not found" > $@; \
	fi

$(IMAGE_A) : $(embox_o) $$(common_prereqs)
	@$(RM) $@
	$(AR) rcs $@ $<

$(IMAGE_LINKED_A) : $(image_relocatable_o) $$(common_prereqs)
	@$(RM) $@
	$(AR) rcs $@ $<
