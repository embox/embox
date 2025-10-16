#
#   Date: Jun 22, 2012
# Author: Eldar Abusalimov
#

ifeq ($(STAGE),1)
embox_o   := $(OBJ_DIR)/embox.o
else
embox_o   := $(OBJ_DIR)/embox-2.o
$(embox_o) : $(OBJ_DIR)/embox.o
endif

image_lds := $(OBJ_DIR)/mk/image.lds

.PHONY : all FORCE
all : $(embox_o) $(image_lds)

FORCE :

include mk/flags.mk
include mk/image_lib.mk

.SECONDEXPANSION:

include $(MKGEN_DIR)/include.mk
include $(__include_image)
include $(__include_initfs)
include $(__include)

initfs_cp_prerequisites = $(src_file) $(common_prereqs)

cp_T_if_supported := \
	$(shell $(CP) --version 2>&1 | grep -l GNU >/dev/null && echo -T)

# This rule is necessary because otherwise, when considering a rule for
# creating the $(ROOTFS_DIR) directory (required through $(common_prereqs)
# using the secondarily-expanded order-only '| $(@D)/.' prerequisite),
# the '$(ROOTFS_DIR)/%' rule takes precedence over the proper '%/.' one, since
# the former needs a shorter stem ('.') than the latter ('build/.../rootfs').
# This is reproduced only on GNU Make >= 3.82, because of the change in how
# implicit rule search works in Make.
$(ROOTFS_DIR)/. :
	@mkdir -p $@
$(ROOTFS_DIR)/%/. :
	@mkdir -p $@

$(ROOTFS_DIR)/% :
	$(CP) -r $(cp_T_if_supported) $(src_file) $@$(if \
		$(and $(chmod),$(findstring $(chmod),'')),,;chmod $(chmod) $@)
	@touch $@ # workaround when copying directories
	@find $@ -name .gitkeep -type f -print0 | xargs -0 rm -rf

fmt_line = $(addprefix \$(\n)$(\t)$(\t),$1)

initfs_prerequisites = $(cpio_files) \
	$(wildcard $(USER_ROOTFS_DIR) $(USER_ROOTFS_DIR)/*) $(common_prereqs)
$(ROOTFS_IMAGE) :
	@mkdir -p $(ROOTFS_DIR)
	cd $(ROOTFS_DIR) \
		&& find * -depth -print | $(CPIO) -L --quiet -H newc -o -O $(abspath $@)
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
$(OBJ_DIR)/src/fs/driver/initfs/initfs_image.o : $(ROOTFS_IMAGE)

ifdef __REBUILD_ROOTFS
initfs_cp_prerequisites += FORCE
initfs_prerequisites    += FORCE
endif

# Module-level rules.
module_prereqs = $(o_files) $(a_files) $(common_prereqs)

# Return sections like .bss, .bss._something, etc. But preserve .bss..something
# and .bss.embox.something since it is used for Embox's internal purpuses.
# The same is for text, rodata, data sections.
#
# 1. File
# 2. Section name - text, rodata, data, bss
get_sections = $(filter .$2%, $(filter-out .$2..% .$2.embox%, $(shell $(OBJDUMP) -h $1)))

section_in_region = \
	$(shell grep LDS_SECTION_VMA_$1 $(SRCGEN_DIR)/config.lds.h)

# 1. File name (objects *.o or libraries *.a)
# 2. Section name - text, rodata, data, bss
# 3. Name of section specified with @LinkerSection annonation
rename_section = \
	$(if $(and $3,$(call section_in_region,$3)), \
		$(foreach section, $(call get_sections,$1,$2), \
			$(section)=.$3.$(section).module.$(module_id)), \
		$(foreach section, $(call get_sections,$1,$2), \
			$(section)=$(section).module.$(module_id)) \
	)

# Here we also rename .ARM.exidx section because some ARM none-eabi
# compilers crash with Seg Fault for some reason.
$(OBJ_DIR)/module/% : objcopy_options = \
	$(patsubst %, --rename-section %, \
		$(sort $(foreach f,$(o_files) $(a_files), \
			$(call rename_section,$f,text,$(linker_section_text)) \
			$(call rename_section,$f,rodata,$(linker_section_rodata)) \
			$(call rename_section,$f,data,$(linker_section_data)) \
			$(call rename_section,$f,bss,$(linker_section_bss)) \
			$(call rename_section,$f,ARM.exidx,$(linker_section_arm_exidx)) \
			$(call rename_section,$f,ARM.extab,$(linker_section_arm_extab)) \
	)))

#
# Execv segfault workaround: Due to possibly very long objcopy_options
# we should to pass objcopy_options partially. First 10000 options,
# then next 10000 options and so on.
#
# 1. objcopy options
# 2. File for objcopy
#
do_objcopy = \
	$(eval tmp_file=$2.objcopy_options.txt) \
	$(foreach i,$(shell seq 1 10000 $(words $1)), \
		$(if $(wordlist $(i),$(words $1),$1), \
			$(file >>$(tmp_file), $(wordlist 1,10000,$(wordlist $(i),$(words $1),$1))) \
		) \
	) \
	$(EXTERNAL_MAKE_FLAGS) $(abspath $(ROOT_DIR))/mk/objcopy_helper.sh $(tmp_file) $2; \
	$(RM) $(tmp_file)

ar_prerequisites = $(module_prereqs)
$(OBJ_DIR)/module/%.a : mk/arhelper.mk
	@$(MAKE) -f mk/arhelper.mk TARGET='$@' \
		AR='$(AR)' ARFLAGS='$(ARFLAGS)' \
		A_FILES='$(a_files)' \
		O_FILES='$(o_files)' \
		APP_ID='$(is_app)';
	@$(call do_objcopy, $(objcopy_options), $@)

ld_prerequisites = $(module_prereqs)
obj_build=$(if $(strip $(value mod_postbuild)),$@.build.o,$@)
obj_postbuild=$@
$(OBJ_DIR)/module/%.o :
	$(LD) -r -o $(obj_build) $(ldflags) $(call fmt_line,$(o_files) \
            $(if $(a_files),--whole-archive $(a_files) --no-whole-archive))
	@$(if $(module_id),$(call do_objcopy, $(objcopy_options), $(obj_build)))
	$(mod_postbuild)

# Here goes image creation rules...
#
$(embox_o): ldflags_all = $(LDFLAGS) \
		$(call fmt_line,$(call ld_scripts_flag,$(ld_scripts)))
$(embox_o):
	mkdir -p $(OBJ_DIR)/mk;
	$(ROOT_DIR)/mk/gen_buildinfo.sh > $(OBJ_DIR)/mk/buildinfo.ld;
	$(LD) -r $(ldflags_all) \
		$(call fmt_line,$(ld_objs)) \
		--start-group \
		$(call fmt_line,$(ld_libs)) \
		--end-group \
	-o $@

stages := $(wordlist 1,$(STAGE),1 2)

image_prereqs = $(ld_scripts) $(ld_objs) $(ld_libs) $(common_prereqs)

$(embox_o) : $$(image_prereqs)
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
