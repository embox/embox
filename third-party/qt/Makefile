
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

QT_URL= http://releases.qt-project.org/qt4/source/qt-everywhere-opensource-src-4.8.4.tar.gz
QT_BUNDLE = $(notdir $(QT_URL))
QT_MD5 = 89c5ecba180cae74c66260ac732dc5cb  $(QT_BUNDLE)

QT_CONF_FLAGS += -prefix $(QT_INSTALL_DIR)
QT_CONF_FLAGS += -embedded-lite vnc -little-endian
QT_CONF_FLAGS += -xplatform qws/embox-generic-g++ -static
QT_CONF_FLAGS += -opensource -confirm-license
QT_CONF_FLAGS += -no-pch -fast
QT_CONF_FLAGS += -no-largefile -no-qt3support
QT_CONF_FLAGS += -no-kbd-tty -no-mouse-pc -no-mouse-linuxtp -no-gfx-linuxfb -no-gfx-multiscreen
QT_CONF_FLAGS += -no-feature-TEMPORARYFILE -no-feature-PROCESS -no-feature-FILESYSTEMWATCHER
QT_CONF_FLAGS += -no-feature-SHAREDMEMORY -no-feature-NETWORKINTERFACE -no-feature-HOSTINFO
QT_CONF_FLAGS += -no-feature-QWS_INPUTMETHODS -no-sql-sqlite -D QT_NO_BEARERMANAGEMENT
# examples ?and demos? are not fully compilable
QT_CONF_FLAGS += -nomake examples -nomake demos
# translations cause 'make -q' to fail
QT_CONF_FLAGS += -nomake translations
QT_CONF_FLAGS += -no-exceptions
QT_CONF_FLAGS += -debug
QT_CONF_FLAGS += -qt-gfx-vnc

# configure -embedded x86 -xplatform qws/embox-generic-g++ -no-exceptions -debug -static -little-endian -no-pch -no-largefile -no-qt3support -fast -opensource -confirm-license -no-kbd-tty -no-mouse-pc -no-mouse-linuxtp -no-gfx-linuxfb -no-gfx-multiscreen -qt-gfx-vnc -no-feature-TEMPORARYFILE -no-feature-PROCESS -no-feature-FILESYSTEMWATCHER -no-feature-SHAREDMEMORY -no-feature-NETWORKINTERFACE -no-feature-HOSTINFO -no-feature-QWS_INPUTMETHODS -no-sql-sqlite -D QT_NO_BEARERMANAGEMENT
# # these might be useful if do not want to build examples: "-nomake examples -nomake demos"

# # either this or do not build examples at all
# make sub-translations-make_default-ordered
# # examples ?and demos? are not fully compilable
# make -k
# # either this or do not build examples at all
# make sub-translations-install_subtargets-ordered install_qmake install_mkspecs


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
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-rtti,$(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS)))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))

EMBOX_IMPORTED_MAKEFLAGS += $(filter -j,$(EMBOX_MAKEFLAGS))
ifneq ($(filter -j,$(EMBOX_MAKEFLAGS)),)
EMBOX_IMPORTED_MAKEFLAGS += $(shell nproc)
endif
# no wonder the following doesn't work
# EMBOX_IMPORTED_MAKEFLAGS += $(filter --jobserver-fds=%,$(EMBOX_MAKEFLAGS))

EMBOX_DERIVED_CFLAGS    += $(EMBOX_IMPORTED_CFLAGS)
EMBOX_DERIVED_CXXFLAGS  += $(EMBOX_IMPORTED_CXXFLAGS)
EMBOX_DERIVED_MAKEFLAGS += $(EMBOX_IMPORTED_MAKEFLAGS)


QT_DIR         := $(ROOT_DIR)/build/qt
QT_SRC_DIR     := $(QT_DIR)/source
QT_BUILD_DIR   := $(QT_DIR)/build
QT_INSTALL_DIR := $(QT_DIR)/install

QMAKE_CONF_M4  := $(shell (cd tree && find . -name qmake.conf.m4))
QMAKE_CONF     := $(patsubst %.m4,%,$(QMAKE_CONF_M4))
ifneq ($(words $(QMAKE_CONF_M4)),1)
$(error need exactly one qmake.conf.m4)
endif

PATCH_TXT = qt_patch.txt

EXTRACTED  = $(QT_SRC_DIR)/.extracted
ENVIRONMENT = $(QT_BUILD_DIR)/.environment
CONFIGURED = $(QT_BUILD_DIR)/.configured

.PHONY: all build_and_install configure merge_trees

AT:=

all:

$(QT_BUILD_DIR):
	$(AT)mkdir -p $@

.downloaded:
	$(AT)wget -c $(QT_URL)
	$(AT)(echo '$(QT_MD5)' | md5sum -c)
	$(AT)touch $@

# When necessity to re-patch is detected manual resolution is required.
# We cannot automatically detect if there were or not changes in extracted sources
# and do not want to accidentally remove some possible desired changes.
# Therefore it is left to the discretion of the user.
# HINT: Typically it is good to perform build directory cleanup: 'make clean'
$(EXTRACTED): $(PATCH_TXT) .downloaded
	$(AT) [ ! -e $@ ] || (echo Re-patching needed. Please resolve manually. && false)
	-$(AT)rm -rf $(QT_SRC_DIR)
	$(AT)mkdir -p $(QT_SRC_DIR)
	$(AT)cd $(QT_SRC_DIR) && tar -xzf $(abspath $(QT_BUNDLE))
	$(AT)cd $(QT_SRC_DIR) && mv `ls`/* ./
	$(AT)patch -p1 -d $(QT_SRC_DIR) -N --input=$(abspath $(PATCH_TXT))
	$(AT)cp $(PATCH_TXT) $@

# using -f since 'svn update' causes unlink and 'cp -u' complaints
# http://svn.haxx.se/users/archive-2007-10/0797.shtml
# but typically overwriting is what we want for our part of tree contents
merge_trees: $(EXTRACTED)
	$(AT)cd tree && cp -l -u -f --parents -t $(QT_SRC_DIR)/ `find . -not -path '*/.*' -and -type f`

$(QT_SRC_DIR)/$(QMAKE_CONF_M4): merge_trees

.PHONY: $(ENVIRONMENT)
$(ENVIRONMENT): | $(QT_BUILD_DIR)
	-$(AT)rm $@
	echo "EMBOX_DERIVED_CFLAGS='$(EMBOX_DERIVED_CFLAGS)'"     >>$@
	echo "EMBOX_DERIVED_CXXFLAGS='$(EMBOX_DERIVED_CXXFLAGS)'" >>$@
	echo "QT_CONF_FLAGS='$(QT_CONF_FLAGS)'"                   >>$@

# may be need to force the check whether this file was changed
$(QT_SRC_DIR)/$(QMAKE_CONF): $(QT_SRC_DIR)/$(QMAKE_CONF_M4) $(ENVIRONMENT)
	$(AT) \
		$(shell cat $(ENVIRONMENT)) \
		m4 --prefix-builtins $< >$@.tmp
	$(AT)mv -f $@.tmp $@

# intentionally not taking into account possible changes in $(QMAKE_CONF)
configure: merge_trees $(QT_SRC_DIR)/$(QMAKE_CONF) $(ENVIRONMENT) | $(QT_BUILD_DIR)
	$(AT)cd $(QT_BUILD_DIR) && \
		if (diff $(ENVIRONMENT) $(CONFIGURED) >/dev/null); then \
			echo Nothing to do - no reconfiguration is required.; \
		else \
			([ ! -e $(CONFIGURED) ] || (echo Removing old config ... && make confclean && rm $(CONFIGURED))) && \
			echo Configuring... && \
			MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' \
				$(QT_SRC_DIR)/configure $(QT_CONF_FLAGS) && \
			echo Persisting configuration... && \
			mv -f $(ENVIRONMENT) $(CONFIGURED); \
		fi

build_and_install: configure
	$(AT)cd $(QT_BUILD_DIR) && \
		if make -q; then \
			echo Nothing to do - no rebuild is required.; \
		else \
			echo Building... && \
			(rm -r $(QT_APPS_DIR) || true) && \
			make MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' && \
			make install; \
		fi

all: build_and_install





#TARGET_APPS += examples/declarative/animation/basics/color-animation
#TARGET_APPS += demos/textedit
TARGET_APPS += demos/spreadsheet
TARGET_APPS += examples/animation/animatedtiles
TARGET_APPS += examples/animation/moveblocks
TARGET_APPS += examples/widgets/charactermap

TARGET_QMAKE=$(QT_INSTALL_DIR)/bin/qmake

QT_APPS_DIR = $(QT_DIR)/apps

define BUILD_APP_template
$(QT_APPS_DIR)/$(notdir $(1)):
	$(AT)mkdir -p $$@
$(QT_APPS_DIR)/$(notdir $(1))/Makefile: $(TARGET_QMAKE) | $(QT_APPS_DIR)/$(notdir $(1))
	$(AT)cd $$(dir $$@) && $(TARGET_QMAKE) $(QT_SRC_DIR)/$(1)/$(notdir $(1)).pro
.PHONY: $(QT_APPS_DIR)/$(notdir $(1))/$(notdir $(1))
$(QT_APPS_DIR)/$(notdir $(1))/$(notdir $(1)): $(QT_APPS_DIR)/$(notdir $(1))/Makefile
	$(AT)cd $$(dir $$@) && make MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)'
target_apps: $(QT_APPS_DIR)/$(notdir $(1))/$(notdir $(1))
endef

$(foreach app,$(TARGET_APPS),$(eval $(call BUILD_APP_template,$(app))))

TARGET_APPS_SEQUENCE = $(join $(addsuffix :,$(filter-out $(lastword $(TARGET_APPS)),$(TARGET_APPS))),$(filter-out $(firstword $(TARGET_APPS)),$(TARGET_APPS)))

define TARGET_APPS_DEP_template
$(QT_APPS_DIR)/$(notdir $(2))/$(notdir $(2)): $(QT_APPS_DIR)/$(notdir $(1))/$(notdir $(1))
endef

TARGET_PAIR_DEP_template = $(call TARGET_APPS_DEP_template,$(firstword $(subst :, ,$(1))),$(lastword $(subst :, ,$(1))))

$(foreach pair,$(TARGET_APPS_SEQUENCE),$(eval $(call TARGET_PAIR_DEP_template,$(pair))))

$(TARGET_QMAKE): build_and_install

.PHONY: target_apps

all: target_apps





QT_LINUX_BUILD_DIR   := $(QT_DIR)/build_linux
QT_LINUX_INSTALL_DIR := $(QT_DIR)/install_linux

QT_LINUX_CONF_FLAGS += -prefix $(QT_LINUX_INSTALL_DIR)
QT_LINUX_CONF_FLAGS += -embedded-lite qvfb -little-endian
QT_LINUX_CONF_FLAGS += -xplatform qws/linux-x86-g++
# Static build has problems with linking with qpa platform by default
#QT_LINUX_CONF_FLAGS += -static
QT_LINUX_CONF_FLAGS += -opensource -confirm-license
QT_LINUX_CONF_FLAGS += -fast
# -no-pch
QT_LINUX_CONF_FLAGS += -no-largefile -no-qt3support
QT_LINUX_CONF_FLAGS += -no-kbd-tty -no-mouse-pc -no-mouse-linuxtp -no-gfx-linuxfb
# -no-gfx-multiscreen
QT_LINUX_CONF_FLAGS += -no-feature-TEMPORARYFILE -no-feature-PROCESS -no-feature-FILESYSTEMWATCHER
QT_LINUX_CONF_FLAGS += -no-feature-SHAREDMEMORY -no-feature-NETWORKINTERFACE -no-feature-HOSTINFO
QT_LINUX_CONF_FLAGS += -no-feature-QWS_INPUTMETHODS -no-sql-sqlite -D QT_NO_BEARERMANAGEMENT
# examples ?and demos? are not fully compilable
QT_LINUX_CONF_FLAGS += -nomake examples -nomake demos
# translations cause 'make -q' to fail
QT_LINUX_CONF_FLAGS += -nomake translations
QT_LINUX_CONF_FLAGS += -no-exceptions
QT_LINUX_CONF_FLAGS += -debug
QT_LINUX_CONF_FLAGS += -qt-gfx-vnc -qt-gfx-qvfb -qt-kbd-qvfb -qt-mouse-qvfb
QT_LINUX_CONF_FLAGS += -no-webkit -no-javascript-jit -no-script -no-declarative
# tools are not fully compilable
QT_LINUX_CONF_FLAGS += -nomake tools
# ? add this to embox config ?
QT_LINUX_CONF_FLAGS += -depths 8,16,18,32

LINUX_ENVIRONMENT = $(QT_LINUX_BUILD_DIR)/.environment
LINUX_CONFIGURED = $(QT_LINUX_BUILD_DIR)/.configured

$(QT_LINUX_BUILD_DIR):
	$(AT)mkdir -p $@

.PHONY: $(LINUX_ENVIRONMENT)
$(LINUX_ENVIRONMENT): | $(QT_LINUX_BUILD_DIR)
	-$(AT)rm $@
	echo "QT_LINUX_CONF_FLAGS='$(QT_LINUX_CONF_FLAGS)'" >>$@

linux_configure: merge_trees $(LINUX_ENVIRONMENT) | $(QT_LINUX_BUILD_DIR)
	$(AT)cd $(QT_LINUX_BUILD_DIR) && \
		if (diff $(LINUX_ENVIRONMENT) $(LINUX_CONFIGURED) >/dev/null); then \
			echo Nothing to do - no reconfiguration is required.; \
		else \
			([ ! -e $(LINUX_CONFIGURED) ] || (echo Removing old config ... && make confclean && rm $(LINUX_CONFIGURED))) && \
			echo Configuring... && \
			MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' \
				$(QT_SRC_DIR)/configure $(QT_LINUX_CONF_FLAGS) && \
			echo Persisting configuration... && \
			mv -f $(LINUX_ENVIRONMENT) $(LINUX_CONFIGURED); \
		fi

linux_build_and_install: linux_configure
	$(AT)cd $(QT_LINUX_BUILD_DIR) && \
		if make -q; then \
			echo Nothing to do - no rebuild is required.; \
		else \
			echo Building... && \
			make MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' && \
			make install; \
		fi

all: linux_build_and_install




.NOTPARALLEL:
