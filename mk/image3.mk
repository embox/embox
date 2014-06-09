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

include mk/core/common.mk
include mk/image_lib.mk

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

ld_prerequisites =
ar_prerequisites =
initfs_prerequisites =
initfs_cp_prerequisites =

.SECONDEXPANSION:

include $(MKGEN_DIR)/include.mk
include $(__include_image)
include $(__include_initfs)
include $(__include)

.SECONDARY:
.DELETE_ON_ERROR:

image: $(IMAGE)
image: $(IMAGE_BIN) $(IMAGE_SREC) $(IMAGE_SIZE) $(IMAGE_PIGGY)

ifeq ($(value DISASSEMBLY),y)
image : $(IMAGE_DIS)
endif

ifndef LD_SINGLE_T_OPTION
ld_scripts_flag = $(1:%=-T%)
else
ld_scripts_flag = $(if $(strip $1),-T $1)
endif

# This must be expanded in a secondary expansion context.
# NOTE: must be the last one in a list of prerequisites (contains order-only)
common_prereqs = mk/image2.mk mk/flags.mk $(MKGEN_DIR)/build.mk \
	$(if $(value mk_file),$(mk_file)) \
	| $(if $(value my_file),$(dir $(my_file:%=$(OBJ_DIR)/%)).) $(@D)/.

# Here goes image creation rules...

symbols_pass1_c = $(OBJ_DIR)/symbols_pass1.c
symbols_pass2_c = $(OBJ_DIR)/symbols_pass2.c

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
$(symbols_c_files) : mk/script/nm2c.awk $$(common_prereqs)
$(symbols_c_files) : $$(image_o)
	$(NM) $(NM_OPTS) $< | $(AWK) -f mk/script/nm2c.awk > $@

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
$(shell $(MKDIR) $(OBJ_DIR) 2> /dev/null)
GPATH := $(OBJ_DIR:$(ROOT_DIR)/%=%)
VPATH += $(GPATH)

image_nosymbols_o = $(OBJ_DIR)/image_nosymbols.o
image_pass1_o = $(OBJ_DIR)/image_pass1.o

image_files := $(IMAGE) $(image_nosymbols_o) $(image_pass1_o)

#XXX
FINAL_LINK_WITH_CC ?=
ifeq (1,$(FINAL_LINK_WITH_CC))

FINAL_LDFLAGS ?=
$(image_nosymbols_o): $(image_lds) $(embox_o) $$(common_prereqs)
	$(CC) -Wl,--relax $(FINAL_LDFLAGS) \
	$(embox_o) \
	-Wl,--defsym=__symbol_table=0 \
	-Wl,--defsym=__symbol_table_size=0 \
	-Wl,--cref -Wl,-Map,$@.map \
	-o $@

$(image_pass1_o): $(image_lds) $(embox_o) $(symbols_pass1_a) $$(common_prereqs)
	$(CC) -Wl,--relax $(FINAL_LDFLAGS) \
	$(embox_o) \
	$(symbols_pass1_a) \
	-Wl,--cref -Wl,-Map,$@.map \
	-o $@

$(IMAGE): $(image_lds) $(embox_o) $(symbols_pass2_a) $$(common_prereqs)
	$(CC) -Wl,--relax $(FINAL_LDFLAGS) \
	$(embox_o) \
	$(symbols_pass2_a) \
	-Wl,--cref -Wl,-Map,$@.map \
	-o $@
else

$(image_nosymbols_o): $(image_lds) $(embox_o) $$(common_prereqs)
	$(LD) --relax $(ldflags) \
	-T $(image_lds) \
	$(embox_o) \
	--defsym=__symbol_table=0 \
	--defsym=__symbol_table_size=0 \
	--cref -Map $@.map \
	-o $@

$(image_pass1_o): $(image_lds) $(embox_o) $(symbols_pass1_a) $$(common_prereqs)
	$(LD) --relax $(ldflags) \
	-T $(image_lds) \
	$(embox_o) \
	$(symbols_pass1_a) \
	--cref -Map $@.map \
	-o $@

$(IMAGE): $(image_lds) $(embox_o) $(symbols_pass2_a) $$(common_prereqs)
	$(LD) --relax $(ldflags) \
	-T $(image_lds) \
	$(embox_o) \
	$(symbols_pass2_a) \
	--cref -Map $@.map \
	-o $@
endif

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

ifdef ROOTFS_OUT_DIR

.PHONY : __copy_user_rootfs
all : __copy_user_rootfs $(__cpio_files)

$(__cpio_files) :
	$(foreach f,$(patsubst $(abspath $(ROOTFS_DIR))/%,$(ROOTFS_OUT_DIR)/%,$(abspath $@)), \
		$(CP) -r -T $(src_file) $f; \
		$(foreach c,chmod chown,$(if $(and $($c),$(findstring $($c),'')),,$c $($c) $f;)) \
		$(foreach a,$(strip $(subst ',,$(xattr))), \
			attr -s $(basename $(subst =,.,$a)) -V $(subst .,,$(suffix $(subst =,.,$a))) $f;) \
		find $f -name .gitkeep -type f -print0 | xargs -0 /bin/rm -rf)

__copy_user_rootfs :
	if [ -d $(USER_ROOTFS_DIR) ]; \
	then \
		cd $(USER_ROOTFS_DIR) \
			&& cp -r * $(abspath $(ROOTFS_OUT_DIR)); \
	fi
endif

