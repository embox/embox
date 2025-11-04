.PHONY : all __extbld-1 __extbld-2 FORCE
all :

__extbld-1 __extbld-2 :
	@$(if $^,echo 'External build complete',:)

FORCE :

include mk/flags.mk
include mk/image_lib.mk

ifeq ($(ARCH),x86)
AUTOCONF_TARGET_TRIPLET := i386-unknown-none
else
AUTOCONF_TARGET_TRIPLET := $(ARCH)-unknown-none
endif

mod_build_dir = $(EXTERNAL_BUILD_DIR)/$(mod_path)

EXTERNAL_MAKE_FLAGS = \
	MAKEFLAGS= \
	EMBOX_MAKEFLAGS='$(MAKEFLAGS)' \
	EMBOX_IMPORTED_MAKEFLAGS='$(filter -j --jobserver-auth=% --jobserver-fds=%, $(MAKEFLAGS))' \
	MOD_DIR=$(abspath $(dir $(my_file))) \
	MOD_BUILD_DIR=$(abspath $(mod_build_dir)) \
	AUTOCONF_TARGET_TRIPLET=$(AUTOCONF_TARGET_TRIPLET) \
	EMBOX_ARCH=$(ARCH) \
	EMBOX_COMPILER=$(COMPILER) \
	EMBOX_CFLAGS='$(CFLAGS)' \
	EMBOX_CXXFLAGS='$(CXXFLAGS)' \
	EMBOX_CPPFLAGS='$(BUILD_DEPS_CPPFLAGS_BEFORE) $(EMBOX_EXPORT_CPPFLAGS) $(BUILD_DEPS_CPPFLAGS_AFTER)' \
	EMBOX_LDFLAGS='$(LDFLAGS) $(BUILD_DEPS_LDFLAGS)' \
	EMBOX_DEPS_CPPFLAGS_BEFORE='$(BUILD_DEPS_CPPFLAGS_BEFORE)' \
	EMBOX_DEPS_CPPFLAGS_AFTER='$(BUILD_DEPS_CPPFLAGS_AFTER)' \
	EMBOX_MODULE_CPPFLAGS='$(MODULE_CPPFLAGS)'

#
# We use '+', because we want to call external build as recursive sub-make.
# From https://www.gnu.org/software/make/manual/html_node/MAKE-Variable.html:
#
# "This special feature is only enabled if the MAKE variable appears directly
# in the recipe: it does not apply if the MAKE variable is referenced through
# expansion of another variable. In the latter case you must use the '+' token
# to get these special effects."
#
# Since $(MAKE) here is referenced through $(EXTERNAL_MAKE) it's fine to add
# additional '+'.
#
EXTERNAL_MAKE = \
	+$(MAKE) -C $(dir $(my_file)) -f $(ROOT_DIR)/mk/extbld/build.mk $(EXTERNAL_MAKE_FLAGS)

# XXX
EXTERNAL_MAKE_QT = \
	$(MKDIR) $(mod_build_dir) && \
	$(CP) $(EXTERNAL_BUILD_DIR)/third_party/qt/core/install/.qmake.cache $(mod_build_dir) && \
	$(EXTERNAL_BUILD_DIR)/third_party/qt/core/install/bin/qmake \
		INCLUDEPATH+='$(subst -I,,$(BUILD_DEPS_CPPFLAGS_BEFORE) $(BUILD_DEPS_CPPFLAGS_AFTER))' \
		LIBS+='$(BUILD_DEPS_LDFLAGS)' \
		$${TARGET:-$(dir $(my_file))} \
		-o $(abspath $(mod_build_dir))/Makefile && \
	$(MAKE) -C $(mod_build_dir) $(EXTERNAL_MAKE_FLAGS)

.SECONDEXPANSION:

include $(MKGEN_DIR)/include.mk
include $(filter %.extbld_rule.mk,$(__include))
