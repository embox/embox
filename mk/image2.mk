#
#   Date: Jun 22, 2012
# Author: Eldar Abusalimov
#

embox_o   := $(OBJ_DIR)/embox.o
image_lds := $(OBJ_DIR)/mk/image.lds

.PHONY : all FORCE
all : $(embox_o) $(image_lds)

FORCE :

include mk/core/common.mk
include mk/image_lib.mk

include $(MKGEN_DIR)/build.mk

include mk/flags.mk # It must be included after a user-defined config.

.SECONDEXPANSION:

include $(MKGEN_DIR)/include.mk
include $(__include_image)
include $(__include_initfs)
include $(__include)

initfs_cp_prerequisites = $(src_file) $(common_prereqs)
$(ROOTFS_DIR)/%/. : | $(ROOTFS_DIR)/.
	@mkdir -p $(@D)
	@touch $@

$(ROOTFS_DIR)/% : | $(ROOTFS_DIR)/.
	$(CP) -r -T $(src_file) $@$(foreach c,chmod chown,$(if \
		$(and $($c),$(findstring $($c),'')),,;$c $($c) $@))
	@touch $@ # workaround when copying directories
	@find $@ -name .gitkeep -type f -print0 | xargs -0 /bin/rm -rf

$(ROOTFS_DIR)/. :
	@$(MKDIR) $(@D)

fmt_line = $(addprefix \$(\n)$(\t)$(\t),$1)

initfs_prerequisites = $(cpio_files) \
	$(wildcard $(USER_ROOTFS_DIR) $(USER_ROOTFS_DIR)/*) $(common_prereqs)
$(ROOTFS_IMAGE) : rel_cpio_files = \
		$(patsubst $(abspath $(ROOTFS_DIR))/%,%,$(abspath $(cpio_files)))
$(ROOTFS_IMAGE) : | $(ROOTFS_DIR)/.
$(ROOTFS_IMAGE) :
	cd $(ROOTFS_DIR) \
		&& find $(rel_cpio_files) -depth -print | $(CPIO) -L --quiet -H newc -o -O $(abspath $@)
	if [ -d $(USER_ROOTFS_DIR) ]; \
	then \
		cd $(USER_ROOTFS_DIR) \
			&& find * -depth -print | $(CPIO) -L --quiet -H newc -o -A -O $(abspath $@); \
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

ifdef __REBUILD_ROOTFS
initfs_cp_prerequisites += FORCE
initfs_prerequisites    += FORCE
endif

# Module-level rules.
module_prereqs = $(o_files) $(a_files) $(common_prereqs)

$(OBJ_DIR)/module/% : objcopy_flags = \
	$(foreach s,rodata data bss,--rename-section .$s=.$s.module.$(module_id))

ar_prerequisites = $(module_prereqs)
$(OBJ_DIR)/module/%.a : mk/arhelper.mk
	@$(MAKE) -f mk/arhelper.mk TARGET='$@' \
		AR='$(AR)' ARFLAGS='$(ARFLAGS)' \
		A_FILES='$(a_files)' \
		O_FILES='$(o_files)' \
		APP_ID='$(is_app)' $(if $(is_app), \
			OBJCOPY='$(OBJCOPY)' OBJCOPYFLAGS='$(objcopy_flags)')

ld_prerequisites = $(module_prereqs)
$(OBJ_DIR)/module/%.o :
	$(LD) -r -o $@ $(ldflags) $(call fmt_line,$(o_files) \
            $(if $(a_files),--whole-archive $(a_files) --no-whole-archive))
	$(if $(module_id),$(OBJCOPY) $(objcopy_flags) $@)


# Here goes image creation rules...
#
# workaround to get VPATH and GPATH to work with an OBJ_DIR.
$(shell $(MKDIR) $(OBJ_DIR) 2> /dev/null)
GPATH := $(OBJ_DIR:$(ROOT_DIR)/%=%)
VPATH += $(GPATH)

$(embox_o): ldflags_all = $(LDFLAGS) \
		$(call fmt_line,$(call ld_scripts_flag,$(ld_scripts)))
$(embox_o): $$(common_prereqs)
	$(LD) -r $(ldflags_all) \
		$(call fmt_line,$(ld_objs)) \
		--start-group \
		$(call fmt_line,$(ld_libs)) \
		--end-group \
	--cref -Map $@.map \
	-o $@

stages := $(wordlist 1,$(STAGE),1 2)

$(embox_o) : $(__image_prerequisities)
$(embox_o) : mk_file = $(__image_mk_file)
$(embox_o) : ld_scripts = $(__image_ld_scripts1) # TODO check this twice
$(embox_o) : ld_objs = $(foreach s,$(stages),$(__image_ld_objs$s))
$(embox_o) : ld_libs = $(foreach s,$(stages),$(__image_ld_libs$s))

$(image_lds) : $$(common_prereqs)
$(image_lds) : flags_before :=
$(image_lds) : flags = \
		$(addprefix -include ,$(wildcard \
			$(SRC_DIR)/arch/$(ARCH)/embox.lds.S \
			$(if $(value PLATFORM), \
				$(PLATFORM_DIR)/$(PLATFORM)/arch/$(ARCH)/platform.lds.S)))
-include $(image_lds).d

image_prerequisites = $(mk_file) \
	$(ld_scripts) $(ld_objs) $(ld_libs)
