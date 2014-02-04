
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

ifeq (arm,$(EMBOX_ARCH))
ARCH=arm
#ToDo: detect endianness
EMBOX_IMPORTED_CROSS_COMPILE_FLAGS += -little-endian
else ifeq (x86,$(EMBOX_ARCH))
ARCH=i386
EMBOX_IMPORTED_CROSS_COMPILE_FLAGS += -little-endian
# it is temporary hack to prevent building Qt on ARM until frame buffer is not supported
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif

ifneq (,$(EMBOX_CROSS_COMPILE))
EMBOX_IMPORTED_CROSS_COMPILE_FLAGS += -device-option CROSS_COMPILE=$(EMBOX_CROSS_COMPILE)
endif

AT=
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj

EMBOX_IMPORTED_CPPFLAGS += -I$(ROOT_DIR)/build/stlport/include/stlport
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))
#EMBOX_IMPORTED_CPPFLAGS += $(filter -D"__impl_x%,$(EMBOX_CPPFLAGS))
# there is just no way to pass it through right :)
#EMBOX_IMPORTED_CPPFLAGS += "-D\'__impl_x(path)=<../path>\'"

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)

EMBOX_IMPORTED_CFLAGS   += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -mno-unaligned-access,$(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS)))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CXXFLAGS))


EMBOX_IMPORTED_MAKEFLAGS += $(filter -j,$(EMBOX_MAKEFLAGS))
ifneq ($(filter -j,$(EMBOX_MAKEFLAGS)),)
EMBOX_IMPORTED_MAKEFLAGS += $(shell nproc)
endif
# no wonder the following doesn't work
# EMBOX_IMPORTED_MAKEFLAGS += $(filter --jobserver-fds=%,$(EMBOX_MAKEFLAGS))

EMBOX_DERIVED_CFLAGS        += $(EMBOX_IMPORTED_CFLAGS)
EMBOX_DERIVED_CXXFLAGS      += $(EMBOX_IMPORTED_CXXFLAGS)
EMBOX_DERIVED_MAKEFLAGS     += $(EMBOX_IMPORTED_MAKEFLAGS)
#EMBOX_DERIVED_CROSS_COMPILE += $(EMBOX_CROSS_COMPILE)

QT_DIR               := $(ROOT_DIR)/build/qt
QT_SRC_DIR           := $(QT_DIR)/source
QT_COMPONENTS_DIR_EMBOX = $(QT_DIR)/apps_embox

QT_BUILD_DIR_EMBOX   := $(QT_DIR)/build_embox
QT_INSTALL_DIR_EMBOX := $(QT_DIR)/install_embox

#TARGET:
#	cd $(QT_SRC_DIR)/$(dir $(TARGET)) && \
#		mkdir -p "$(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(TARGET))" && \
#		TARGET_QMAKE_EMBOX;


# Component compilation template
# arguments to the template
# 1: full path of the component's source directory
# 2: platform, expected values are 'EMBOX' or 'LINUX'
# 3: install flag: 'install' or ''
define BUILD_COMPONENT_template
$(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1)):
	$(AT)mkdir -p $$@
.PHONY: $(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))/$(notdir $(1))
ifeq ($(3),install)
$(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))/Makefile: $(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))/.qmake.cache
$(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))/.qmake.cache: $(QT_BUILD_DIR_$(2))/.qmake.cache
	$(AT)cp $$< $$@
endif
$(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))/Makefile: $$(TARGET_QMAKE_EMBOX) | $(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))
	$(AT)cd $$(dir $$@) && $$(TARGET_QMAKE_EMBOX) $(1)/$(notdir $(1)).pro
$(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))/$(notdir $(1)): $(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))/Makefile
ifneq ($(3),install)
	$(AT)cd $$(dir $$@) && make MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)'
else
	$(AT)cd $$(dir $$@) && \
		if make -q; then \
			echo Nothing to do for $(notdir $(1)).; \
		else \
			make MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' && \
			make install; \
		fi
endif
target_apps_EMBOX: $(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))/$(notdir $(1))
endef

# Arguments transformer for the component compilation template
# 1: Short path to the component with optional ':install' suffix
# 2: platform, expected values are 'EMBOX' or 'LINUX'
BUILD_COMPONENT_FUNC = $(call BUILD_COMPONENT_template,$(call COMP_fullpath,$(call COMP_path,$(1))),$(2),$(call COMP_install,$(1)))
COMP_path     = $(word 1,$(subst :, ,$(1)))
COMP_install  = $(word 2,$(subst :, ,$(1)))
COMP_fullpath = $(if $(shell find . -path './$(1)'),$(abspath $(1)),$(QT_SRC_DIR)/$(1))

# Component sequence temlpate
# arguments to the template
# 1: path to the preceeding component
# 2: path to the following component
# 3: platform, expected values are 'EMBOX' or 'LINUX'
define TARGET_COMPONENT_DEP_template
$(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(2))/$(notdir $(2)): $(QT_COMPONENTS_DIR_EMBOX)/$(notdir $(1))/$(notdir $(1))
endef

TARGET_PAIR_DEP_template = $(call TARGET_COMPONENT_DEP_template,$(firstword $(subst :, ,$(1))),$(lastword $(subst :, ,$(1))),$(2))

# argument: 1: platform, expected values are 'EMBOX' or 'LINUX'
define TARGET_COMPONENTS_SEQUENCE_PLATFORM
TARGET_COMPS =  $$(foreach comp,$$(TARGET_COMPONENTS_EMBOX),$$(call COMP_path,$$(comp)))
TARGET_COMPS_SEQUENCE = $$(join $$(addsuffix :,$$(filter-out $$(lastword $$(TARGET_APPS)),$$(TARGET_APPS))),$$(filter-out $$(firstword $$(TARGET_APPS)),$$(TARGET_APPS)))
$$(foreach pair,$$(TARGET_COMPS_SEQUENCE),$$(eval $$(call TARGET_PAIR_DEP_template,$$(pair),$(1))))
endef

define TARGET_COMPONENTS_COMPOSITE
$$(foreach app,$$(TARGET_COMPONENTS_EMBOX),$$(eval $$(call BUILD_COMPONENT_FUNC,$$(app),$(1))))
$$(eval $$(call TARGET_COMPONENTS_SEQUENCE_PLATFORM,$(1)))
endef

# plugins strictly before importing library both with ':install'
# everything else before applications
$(EMBOX_TARGET): target_apps_EMBOX
TARGET_APPS=$(EMBOX_TARGET)
TARGET_COMPONENTS_EMBOX += $(TARGET_APPS)

TARGET_QMAKE_EMBOX       =$(QT_INSTALL_DIR_EMBOX)/bin/qmake


$(eval $(call TARGET_COMPONENTS_COMPOSITE,EMBOX))

.NOTPARALLEL:
