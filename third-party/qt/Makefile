
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

DOWNLOAD_DIR:= $(ROOT_DIR)/download

QT_URL:=http://download.qt-project.org/official_releases/qt/4.8/4.8.5/qt-everywhere-opensource-src-4.8.5.tar.gz
QT_MD5:= 1864987bdbb2f58f8ae8b350dfdbe133
QT_BUNDLE:=$(DOWNLOAD_DIR)/$(notdir $(QT_URL))

QT_CONF_FLAGS_EMBOX += -hostprefix $(QT_INSTALL_DIR_EMBOX)
# This will be needed to use runtime resources, e.g. fonts
#QT_CONF_FLAGS_EMBOX += -prefix /qt
QT_CONF_FLAGS_EMBOX += -arch i386
QT_CONF_FLAGS_EMBOX += -embedded-lite vnc -little-endian
QT_CONF_FLAGS_EMBOX += -xplatform qws/embox-generic-g++ -static
QT_CONF_FLAGS_EMBOX += -opensource -confirm-license
QT_CONF_FLAGS_EMBOX += -no-pch -fast
QT_CONF_FLAGS_EMBOX += -no-largefile -no-qt3support
QT_CONF_FLAGS_EMBOX += -no-kbd-tty -no-mouse-pc -no-mouse-linuxtp -no-gfx-linuxfb -no-gfx-multiscreen
QT_CONF_FLAGS_EMBOX += -no-feature-TEMPORARYFILE     -D QT_NO_TEMPORARYFILE
QT_CONF_FLAGS_EMBOX += -no-feature-PROCESS           -D QT_NO_PROCESS
#QT_CONF_FLAGS_EMBOX += -no-feature-FILESYSTEMWATCHER -D QT_NO_FILESYSTEMWATCHER
QT_CONF_FLAGS_EMBOX += -no-feature-SHAREDMEMORY      -D QT_NO_SHAREDMEMORY
QT_CONF_FLAGS_EMBOX += -no-feature-NETWORKINTERFACE  -D QT_NO_NETWORKINTERFACE
QT_CONF_FLAGS_EMBOX += -no-feature-HOSTINFO          -D QT_NO_HOSTINFO
QT_CONF_FLAGS_EMBOX += -no-feature-QWS_INPUTMETHODS  -D QT_NO_QWS_INPUTMETHODS
# Necessity to add this appeared after something was added, maybe -no-freetype
QT_CONF_FLAGS_EMBOX += -D QT_NO_IM
QT_CONF_FLAGS_EMBOX += -D Q_NO_POSIX_SIGNALS
# Disabling threading and concurrency stuff is a dead end :(
#QT_CONF_FLAGS_EMBOX += -D QT_NO_THREAD
# Automatically defined by NO_CONCURRENT
#QT_CONF_FLAGS_EMBOX += -D QT_NO_QFUTURE
#QT_CONF_FLAGS_EMBOX += -no-feature-CONCURRENT        -D QT_NO_CONCURRENT
# Should be transitively disabled by NO_LIBRARY according to qfeatures.txt
QT_CONF_FLAGS_EMBOX += -D QT_NO_BEARERMANAGEMENT
# This flag looks suspicious: need to check how it works on Linux
QT_CONF_FLAGS_EMBOX += -D QT_NO_DYNAMIC_LIBRARY
#QT_CONF_FLAGS_EMBOX += -D QT_NO_LIBRARY_UNLOAD
# QT_NO_LIBRARY disables static plugins so not using it
#QT_CONF_FLAGS_EMBOX += -no-feature-LIBRARY           -D QT_NO_LIBRARY
QT_CONF_FLAGS_EMBOX += -no-sql-sqlite
# examples ?and demos? are not fully compilable
QT_CONF_FLAGS_EMBOX += -nomake examples -nomake demos
# translations cause 'make -q' to fail
QT_CONF_FLAGS_EMBOX += -nomake translations
ifeq ($(filter -fno-exceptions,$(EMBOX_CXXFLAGS)),-fno-exceptions)
QT_CONF_FLAGS_EMBOX += -no-exceptions
endif
QT_CONF_FLAGS_EMBOX += -debug
QT_CONF_FLAGS_EMBOX += -qt-gfx-vnc
QT_CONF_FLAGS_EMBOX += -no-freetype



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

EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS)))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))


EMBOX_IMPORTED_MAKEFLAGS += $(filter -j,$(EMBOX_MAKEFLAGS))
ifneq ($(filter -j,$(EMBOX_MAKEFLAGS)),)
EMBOX_IMPORTED_MAKEFLAGS += $(shell nproc)
endif
# no wonder the following doesn't work
# EMBOX_IMPORTED_MAKEFLAGS += $(filter --jobserver-fds=%,$(EMBOX_MAKEFLAGS))

EMBOX_DERIVED_CFLAGS    += $(EMBOX_IMPORTED_CFLAGS)
EMBOX_DERIVED_CXXFLAGS  += $(EMBOX_IMPORTED_CXXFLAGS)
EMBOX_DERIVED_MAKEFLAGS += $(EMBOX_IMPORTED_MAKEFLAGS)

QT_DIR               := $(ROOT_DIR)/build/qt
QT_SRC_DIR           := $(QT_DIR)/source

QT_BUILD_DIR_EMBOX   := $(QT_DIR)/build_embox
QT_INSTALL_DIR_EMBOX := $(QT_DIR)/install_embox

QMAKE_CONF_M4  := $(shell (cd tree && find . -name qmake.conf.m4))
QMAKE_CONF     := $(patsubst %.m4,%,$(QMAKE_CONF_M4))
ifneq ($(words $(QMAKE_CONF_M4)),1)
$(error need exactly one qmake.conf.m4)
endif

PATCH_TXT = qt_patch.txt

EXTRACTED  = $(QT_SRC_DIR)/.extracted
DOWNLOADED = $(QT_DIR)/.downloaded
ENVIRONMENT_EMBOX = $(QT_BUILD_DIR_EMBOX)/.environment
CONFIGURED_EMBOX = $(QT_BUILD_DIR_EMBOX)/.configured

.PHONY: all build_and_install configure merge_trees

AT:=

all:

$(QT_BUILD_DIR_EMBOX):
	$(AT)mkdir -p $@

$(DOWNLOADED):
	$(AT)wget -P $(DOWNLOAD_DIR) -c $(QT_URL)
	$(AT)(echo '$(QT_MD5) $(QT_BUNDLE)' | md5sum -c)
	$(AT)touch $@

# When necessity to re-patch is detected manual resolution is required.
# We cannot automatically detect if there were or not changes in extracted sources
# and do not want to accidentally remove some possible desired changes.
# Therefore it is left to the discretion of the user.
# HINT: Typically it is good to perform build directory cleanup: 'make clean'
$(EXTRACTED): $(PATCH_TXT) $(DOWNLOADED)
	$(AT) [ ! -e $@ ] || (echo Re-patching is needed. Please resolve manually. && false)
	-$(AT)rm -rf $(QT_SRC_DIR)
	$(AT)mkdir -p $(QT_SRC_DIR)
	$(AT)tar -C $(QT_SRC_DIR) -xzf $(abspath $(QT_BUNDLE)) --strip-component=1
	$(AT)patch -p1 -d $(QT_SRC_DIR) -N --input=$(abspath $(PATCH_TXT))
	$(AT)touch $@

# using -f since 'svn update' causes unlink and 'cp -u' complaints
# http://svn.haxx.se/users/archive-2007-10/0797.shtml
# but typically overwriting is what we want for our part of tree contents
merge_trees: $(EXTRACTED)
	$(AT)cd tree && cp -l -u -f --parents -t $(QT_SRC_DIR)/ `find . -not -path '*/.*' -and -type f`

$(QT_SRC_DIR)/$(QMAKE_CONF_M4): merge_trees

.PHONY: $(ENVIRONMENT_EMBOX)
$(ENVIRONMENT_EMBOX): | $(QT_BUILD_DIR_EMBOX)
	-$(AT)rm $@
	echo "EMBOX_DERIVED_CFLAGS='$(EMBOX_DERIVED_CFLAGS)'"     >>$@
	echo "EMBOX_DERIVED_CXXFLAGS='$(EMBOX_DERIVED_CXXFLAGS)'" >>$@
	echo "QT_CONF_FLAGS_EMBOX='$(QT_CONF_FLAGS_EMBOX)'"                   >>$@

# may be need to force the check whether this file was changed
$(QT_SRC_DIR)/$(QMAKE_CONF): $(QT_SRC_DIR)/$(QMAKE_CONF_M4) $(ENVIRONMENT_EMBOX)
	$(AT) \
		$(shell cat $(ENVIRONMENT_EMBOX)) \
		m4 --prefix-builtins $< >$@.tmp
	$(AT)mv -f $@.tmp $@

# intentionally not taking into account possible changes in $(QMAKE_CONF)
configure: merge_trees $(QT_SRC_DIR)/$(QMAKE_CONF) $(ENVIRONMENT_EMBOX) | $(QT_BUILD_DIR_EMBOX)
	$(AT)cd $(QT_BUILD_DIR_EMBOX) && \
		if (diff $(ENVIRONMENT_EMBOX) $(CONFIGURED_EMBOX) >/dev/null); then \
			echo Nothing to do - no reconfiguration is required.; \
		else \
			([ ! -e $(CONFIGURED_EMBOX) ] || (echo Removing old config ... && make confclean && rm $(CONFIGURED_EMBOX))) && \
			echo Configuring... && \
			MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' \
				$(QT_SRC_DIR)/configure $(QT_CONF_FLAGS_EMBOX) && \
			echo Persisting configuration... && \
			mv -f $(ENVIRONMENT_EMBOX) $(CONFIGURED_EMBOX); \
		fi

build_and_install: configure
	$(AT)cd $(QT_BUILD_DIR_EMBOX) && \
		if make -q; then \
			echo Nothing to do - no rebuild is required.; \
		else \
			echo Building... && \
			(rm -r $(QT_COMPONENTS_DIR_EMBOX) || true) && \
			make MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' && \
			make install && \
			mkdir -p $(ROOT_DIR)/conf/rootfs && \
			cp -l -f -u `find $(QT_INSTALL_DIR_EMBOX)/lib/fonts/ -type f` $(ROOT_DIR)/conf/rootfs/fonts; \
		fi

$(ROOTFS_IMAGE): build_and_install

#TARGET_APPS += examples/declarative/animation/basics/color-animation
#TARGET_APPS += demos/textedit
TARGET_APPS += demos/spreadsheet
TARGET_APPS += examples/animation/animatedtiles
TARGET_APPS += examples/animation/moveblocks
TARGET_APPS += examples/widgets/charactermap
TARGET_APPS += examples/texteditor



# Component compilation template
# arguments to the template
# 1: full path of the component's source directory
# 2: platform, expected values are 'EMBOX' or 'LINUX'
# 3: install flag: 'install' or ''
define BUILD_COMPONENT_template
$(QT_COMPONENTS_DIR_$(2))/$(notdir $(1)):
	$(AT)mkdir -p $$@
.PHONY: $(QT_COMPONENTS_DIR_$(2))/$(notdir $(1))/$(notdir $(1))
ifeq ($(3),install)
$(QT_COMPONENTS_DIR_$(2))/$(notdir $(1))/Makefile: $(QT_COMPONENTS_DIR_$(2))/$(notdir $(1))/.qmake.cache
$(QT_COMPONENTS_DIR_$(2))/$(notdir $(1))/.qmake.cache: $(QT_BUILD_DIR_$(2))/.qmake.cache
	$(AT)cp $$< $$@
endif
$(QT_COMPONENTS_DIR_$(2))/$(notdir $(1))/Makefile: $$(TARGET_QMAKE_$(2)) | $(QT_COMPONENTS_DIR_$(2))/$(notdir $(1))
	$(AT)cd $$(dir $$@) && $$(TARGET_QMAKE_$(2)) $(1)/$(notdir $(1)).pro
$(QT_COMPONENTS_DIR_$(2))/$(notdir $(1))/$(notdir $(1)): $(QT_COMPONENTS_DIR_$(2))/$(notdir $(1))/Makefile
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
target_apps_$(2): $(QT_COMPONENTS_DIR_$(2))/$(notdir $(1))/$(notdir $(1))
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
$(QT_COMPONENTS_DIR_$(3))/$(notdir $(2))/$(notdir $(2)): $(QT_COMPONENTS_DIR_$(3))/$(notdir $(1))/$(notdir $(1))
endef

TARGET_PAIR_DEP_template = $(call TARGET_COMPONENT_DEP_template,$(firstword $(subst :, ,$(1))),$(lastword $(subst :, ,$(1))),$(2))

# argument: 1: platform, expected values are 'EMBOX' or 'LINUX'
define TARGET_COMPONENTS_SEQUENCE_PLATFORM
TARGET_COMPS =  $$(foreach comp,$$(TARGET_COMPONENTS_$(1)),$$(call COMP_path,$$(comp)))
TARGET_COMPS_SEQUENCE = $$(join $$(addsuffix :,$$(filter-out $$(lastword $$(TARGET_APPS)),$$(TARGET_APPS))),$$(filter-out $$(firstword $$(TARGET_APPS)),$$(TARGET_APPS)))
$$(foreach pair,$$(TARGET_COMPS_SEQUENCE),$$(eval $$(call TARGET_PAIR_DEP_template,$$(pair),$(1))))
endef

define TARGET_COMPONENTS_COMPOSITE
$$(foreach app,$$(TARGET_COMPONENTS_$(1)),$$(eval $$(call BUILD_COMPONENT_FUNC,$$(app),$(1))))
$$(eval $$(call TARGET_COMPONENTS_SEQUENCE_PLATFORM,$(1)))
endef



# plugins strictly before importing library both with ':install'
# everything else before applications
TARGET_COMPONENTS_EMBOX += src/plugins/platforms/minimal:install
TARGET_COMPONENTS_EMBOX += src/plugins/platforms/vnc:install
TARGET_COMPONENTS_EMBOX += libs/EmboxAutoPluginImporter:install
TARGET_COMPONENTS_EMBOX += src/plugins/platforms/emboxvc:install
TARGET_COMPONENTS_EMBOX += $(TARGET_APPS)

TARGET_QMAKE_EMBOX       =$(QT_INSTALL_DIR_EMBOX)/bin/qmake

QT_COMPONENTS_DIR_EMBOX = $(QT_DIR)/apps_embox

$(eval $(call TARGET_COMPONENTS_COMPOSITE,EMBOX))

$(TARGET_QMAKE_EMBOX): build_and_install

.PHONY: target_apps_EMBOX

all: target_apps_EMBOX





QT_BUILD_DIR_LINUX   := $(QT_DIR)/build_linux
QT_INSTALL_DIR_LINUX := $(QT_DIR)/install_linux

QT_CONF_FLAGS_LINUX += -prefix $(QT_INSTALL_DIR_LINUX)
QT_CONF_FLAGS_LINUX += -embedded-lite qvfb -little-endian
QT_CONF_FLAGS_LINUX += -xplatform qws/linux-x86-g++
# Static build has problems with linking with qpa platform by default
#QT_CONF_FLAGS_LINUX += -static
QT_CONF_FLAGS_LINUX += -opensource -confirm-license
QT_CONF_FLAGS_LINUX += -fast
# -no-pch
QT_CONF_FLAGS_LINUX += -no-largefile -no-qt3support
QT_CONF_FLAGS_LINUX += -no-kbd-tty -no-mouse-pc -no-mouse-linuxtp -no-gfx-linuxfb
# -no-gfx-multiscreen
QT_CONF_FLAGS_LINUX += -no-feature-TEMPORARYFILE
QT_CONF_FLAGS_LINUX += -no-feature-PROCESS
#QT_CONF_FLAGS_LINUX += -no-feature-FILESYSTEMWATCHER
QT_CONF_FLAGS_LINUX += -no-feature-SHAREDMEMORY
QT_CONF_FLAGS_LINUX += -no-feature-NETWORKINTERFACE
QT_CONF_FLAGS_LINUX += -no-feature-HOSTINFO
QT_CONF_FLAGS_LINUX += -no-feature-QWS_INPUTMETHODS
QT_CONF_FLAGS_LINUX += -D QT_NO_BEARERMANAGEMENT
# Doesn't make sense to do it on Linux
#QT_CONF_FLAGS_LINUX += -no-feature-LIBRARY
#QT_CONF_FLAGS_LINUX += -D QT_NO_DYNAMIC_LIBRARY
#QT_CONF_FLAGS_LINUX += -D QT_NO_LIBRARY_UNLOAD
# examples ?and demos? are not fully compilable
QT_CONF_FLAGS_LINUX += -no-sql-sqlite
QT_CONF_FLAGS_LINUX += -nomake examples -nomake demos
# translations cause 'make -q' to fail
QT_CONF_FLAGS_LINUX += -nomake translations
ifeq ($(filter -fno-exceptions,$(EMBOX_CXXFLAGS)),-fno-exceptions)
QT_CONF_FLAGS_LINUX += -no-exceptions
endif
QT_CONF_FLAGS_LINUX += -debug
QT_CONF_FLAGS_LINUX += -qt-gfx-vnc -qt-gfx-qvfb -qt-kbd-qvfb -qt-mouse-qvfb
QT_CONF_FLAGS_LINUX += -no-webkit -no-javascript-jit -no-script -no-declarative
# tools are not fully compilable
QT_CONF_FLAGS_LINUX += -nomake tools
# ? add this to embox config ?
QT_CONF_FLAGS_LINUX += -depths 8,16,18,32
QT_CONF_FLAGS_LINUX += -no-opengl -no-openssl
QT_CONF_FLAGS_LINUX += -no-freetype
QT_CONF_FLAGS_LINUX += -D QT_OVERRIDE_NO_WAYLAND
#QT_CONF_FLAGS_LINUX += -D QT_NO_WAYLAND_XKB

ENVIRONMENT_LINUX = $(QT_BUILD_DIR_LINUX)/.environment
CONFIGURED_LINUX = $(QT_BUILD_DIR_LINUX)/.configured

$(QT_BUILD_DIR_LINUX):
	$(AT)mkdir -p $@

.PHONY: $(ENVIRONMENT_LINUX)
$(ENVIRONMENT_LINUX): | $(QT_BUILD_DIR_LINUX)
	-$(AT)rm $@
	echo "QT_CONF_FLAGS_LINUX='$(QT_CONF_FLAGS_LINUX)'" >>$@

linux_configure: merge_trees $(ENVIRONMENT_LINUX) | $(QT_BUILD_DIR_LINUX)
	$(AT)cd $(QT_BUILD_DIR_LINUX) && \
		if (diff $(ENVIRONMENT_LINUX) $(CONFIGURED_LINUX) >/dev/null); then \
			echo Nothing to do - no reconfiguration is required.; \
		else \
			([ ! -e $(CONFIGURED_LINUX) ] || (echo Removing old config ... && make confclean && rm $(CONFIGURED_LINUX))) && \
			echo Configuring... && \
			MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' \
				$(QT_SRC_DIR)/configure $(QT_CONF_FLAGS_LINUX) && \
			echo Persisting configuration... && \
			mv -f $(ENVIRONMENT_LINUX) $(CONFIGURED_LINUX); \
		fi

linux_build_and_install: linux_configure
	$(AT)cd $(QT_BUILD_DIR_LINUX) && \
		if make -q; then \
			echo Nothing to do - no rebuild is required.; \
		else \
			echo Building... && \
			make MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' && \
			make install; \
		fi

#all: linux_build_and_install



# everything else before applications
#TARGET_COMPONENTS_LINUX += src/plugins/platforms/minimal:install
TARGET_COMPONENTS_LINUX += src/plugins/platforms/vnc:install
TARGET_COMPONENTS_LINUX += src/plugins/platforms/qvfb:install
#qwayland from Qt4 source tree depends on wayland 0.8, which is very old
#TARGET_COMPONENTS_LINUX += src/plugins/platforms/wayland:install
#TARGET_COMPONENTS_LINUX += libs/qtwayland/src/plugins/platforms/wayland_common:install
#TARGET_COMPONENTS_LINUX += libs/qtwayland/src/plugins/platforms/qwayland_nogl:install
TARGET_COMPONENTS_LINUX += $(TARGET_APPS)

TARGET_QMAKE_LINUX = $(QT_INSTALL_DIR_LINUX)/bin/qmake

QT_COMPONENTS_DIR_LINUX = $(QT_DIR)/apps_linux

$(eval $(call TARGET_COMPONENTS_COMPOSITE,LINUX))

$(TARGET_QMAKE_LINUX): linux_build_and_install

.PHONY: target_apps_LINUX

#all: target_apps_LINUX



.NOTPARALLEL:
