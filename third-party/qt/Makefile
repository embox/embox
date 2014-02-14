BUILD_DIR      := $(ROOT_DIR)/build
DOWNLOAD_DIR   := $(ROOT_DIR)/download

QT_DIR         := $(BUILD_DIR)/qt
QT_SRC_DIR     := $(QT_DIR)/source
QT_BUILD_DIR   := $(QT_DIR)/build
QT_INSTALL_DIR := $(QT_DIR)/install

DOWNLOADED  := $(QT_DIR)/.downloaded
EXTRACTED   := $(QT_DIR)/.extracted
CONFIGURED  := $(QT_DIR)/.configured
ENVIRONMENT := $(QT_DIR)/.environment

QT_URL    := http://download.qt-project.org/official_releases/qt/4.8/4.8.5/qt-everywhere-opensource-src-4.8.5.tar.gz
QT_MD5    := 1864987bdbb2f58f8ae8b350dfdbe133
QT_BUNDLE := $(DOWNLOAD_DIR)/$(notdir $(QT_URL))

QT_PATCH := qt_patch.txt

define option_get
$(shell echo OPTION_BOOLEAN_third_party__qt__core__$(1) | gcc -P -E -include $(ROOT_DIR)/build/base/src-gen/include/config/third_party/qt/core.h -)
endef

ifeq (arm,$(EMBOX_ARCH))
ARCH_FLAGS   := -arch arm
ENDIAN_FLAGS := -little-endian
else ifeq (x86,$(EMBOX_ARCH))
ARCH_FLAGS   := -arch i386
ENDIAN_FLAGS := -little-endian
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif

CROSS_COMPILER_FLAGS :=
ifneq (,$(EMBOX_CROSS_COMPILE))
CROSS_COMPILER_FLAGS += -device-option CROSS_COMPILE='$(EMBOX_CROSS_COMPILE)'
endif

QT_CONF_FLAGS :=
QT_CONF_FLAGS += -hostprefix $(QT_INSTALL_DIR)
QT_CONF_FLAGS += $(ARCH_FLAGS) $(ENDIAN_FLAGS) $(CROSS_COMPILER_FLAGS)
QT_CONF_FLAGS += -embedded-lite vnc -v
QT_CONF_FLAGS += -xplatform qws/embox-generic-g++ -static
QT_CONF_FLAGS += -opensource -confirm-license
QT_CONF_FLAGS += -no-pch -fast
QT_CONF_FLAGS += -no-largefile -no-qt3support
QT_CONF_FLAGS += -no-kbd-tty -no-mouse-pc -no-mouse-linuxtp \
				 -no-gfx-linuxfb -no-gfx-multiscreen
QT_CONF_FLAGS += -no-feature-TEMPORARYFILE -D QT_NO_TEMPORARYFILE
QT_CONF_FLAGS += -no-feature-PROCESS -D QT_NO_PROCESS
QT_CONF_FLAGS += -no-feature-SHAREDMEMORY -D QT_NO_SHAREDMEMORY
QT_CONF_FLAGS += -no-feature-NETWORKINTERFACE -D QT_NO_NETWORKINTERFACE
QT_CONF_FLAGS += -no-feature-HOSTINFO -D QT_NO_HOSTINFO
QT_CONF_FLAGS += -no-feature-QWS_INPUTMETHODS -D QT_NO_QWS_INPUTMETHODS
ifneq (0,$(call option_get,stl_support))
QT_CONF_FLAGS += -stl
else
QT_CONF_FLAGS += -no-stl -D QT_NO_STL
endif
QT_CONF_FLAGS += -D QT_NO_IM
QT_CONF_FLAGS += -D Q_NO_POSIX_SIGNALS
QT_CONF_FLAGS += -D QT_NO_BEARERMANAGEMENT
QT_CONF_FLAGS += -D QT_NO_DYNAMIC_LIBRARY
QT_CONF_FLAGS += -no-sql-sqlite
QT_CONF_FLAGS += -nomake examples -nomake demos
QT_CONF_FLAGS += -nomake translations
ifneq (,$(filter -fno-exceptions,$(EMBOX_CXXFLAGS)))
QT_CONF_FLAGS += -no-exceptions
endif
QT_CONF_FLAGS += -debug
QT_CONF_FLAGS += -qt-gfx-vnc
QT_CONF_FLAGS += -no-freetype
QT_CONF_FLAGS += -no-xmlpatterns

EMBOX_IMPORTED_CPPFLAGS :=
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))
ifeq (1,$(call option_get,stl_support))
EMBOX_IMPORTED_CPPFLAGS += -I$(ROOT_DIR)/build/stlport/include/stlport
endif

EMBOX_IMPORTED_CFLAGS :=
EMBOX_IMPORTED_CFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CFLAGS += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS :=
EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS))) # remove me
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))

EMBOX_IMPORTED_MAKEFLAGS :=
ifneq (,$(filter -j,$(EMBOX_MAKEFLAGS)))
EMBOX_IMPORTED_MAKEFLAGS += -j $(shell nproc)
endif

EMBOX_DERIVED_CFLAGS    := $(EMBOX_IMPORTED_CFLAGS)
EMBOX_DERIVED_CXXFLAGS  := $(EMBOX_IMPORTED_CXXFLAGS)
EMBOX_DERIVED_MAKEFLAGS := $(EMBOX_IMPORTED_MAKEFLAGS)

QMAKE_CONF_M4  := $(shell (cd tree && find . -name qmake.conf.m4))
QMAKE_CONF     := $(patsubst %.m4,%,$(QMAKE_CONF_M4))
ifneq ($(words $(QMAKE_CONF_M4)),1)
$(error need exactly one qmake.conf.m4)
endif

.PHONY: all build_and_install configure merge_trees

all: build_and_install

$(QT_BUILD_DIR):
	mkdir -p $@

QT_MD5_LINE:=$(QT_MD5)  $(QT_BUNDLE)

$(DOWNLOADED):
	(echo '$(QT_MD5_LINE)' | $(MD5) -c) || \
		(wget -P $(DOWNLOAD_DIR) -c $(QT_URL) && \
			echo '$(QT_MD5_LINE)' | $(MD5) -c)
	mkdir -p $(dir $@)
	touch $@

# When necessity to re-patch is detected manual resolution is required.
# We cannot automatically detect if there were or not changes in extracted sources
# and do not want to accidentally remove some possible desired changes.
# Therefore it is left to the discretion of the user.
# HINT: Typically it is good to perform build directory cleanup: 'make clean'
# The patch is saved to .extracted for the user to be able to compare previous patch with current one.
# In some situations this might be useful.
$(EXTRACTED): $(QT_PATCH) $(DOWNLOADED)
	 [ ! -e $@ ] || (echo Re-patching is needed. Please resolve manually. && false)
	rm -rf $(QT_SRC_DIR)
	mkdir -p $(QT_SRC_DIR)
	tar -C $(QT_SRC_DIR) -xzf $(abspath $(QT_BUNDLE)) --strip-components=1
	patch -p1 -d $(QT_SRC_DIR) -N --input=$(abspath $(QT_PATCH))
	cp $(QT_PATCH) $@

# using -f since 'svn update' causes unlink and 'cp -u' complaints
# http://svn.haxx.se/users/archive-2007-10/0797.shtml
# but typically overwriting is what we want for our part of tree contents
merge_trees: $(EXTRACTED)
	cd tree && cp -l -u -f --parents -t $(QT_SRC_DIR)/ `find . -not -path '*/.*' -and -type f`

$(QT_SRC_DIR)/$(QMAKE_CONF_M4): merge_trees

.PHONY: $(ENVIRONMENT)
$(ENVIRONMENT): | $(QT_BUILD_DIR)
	-rm $@
	echo "EMBOX_DERIVED_CFLAGS='$(EMBOX_DERIVED_CFLAGS)'"               >>$@
	echo "EMBOX_DERIVED_CXXFLAGS='$(EMBOX_DERIVED_CXXFLAGS)'"           >>$@
	#echo "EMBOX_DERIVED_CROSS_COMPILE='$(EMBOX_DERIVED_CROSS_COMPILE)'" >>$@
	echo "QT_CONF_FLAGS='$(QT_CONF_FLAGS)'"                   >>$@

# may be need to force the check whether this file was changed
$(QT_SRC_DIR)/$(QMAKE_CONF): $(QT_SRC_DIR)/$(QMAKE_CONF_M4) $(ENVIRONMENT)
	 \
		$(shell cat $(ENVIRONMENT)) \
		m4 --prefix-builtins $< >$@.tmp
	mv -f $@.tmp $@

# intentionally not taking into account possible changes in $(QMAKE_CONF)
configure: merge_trees $(QT_SRC_DIR)/$(QMAKE_CONF) $(ENVIRONMENT) | $(QT_BUILD_DIR)
	cd $(QT_BUILD_DIR) && \
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
	cd $(QT_BUILD_DIR) && \
		if make -q; then \
			echo Nothing to do - no rebuild is required.; \
		else \
			echo Building... && \
			make MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)' && \
			make install && \
			(cp $(QT_BUILD_DIR)/mkspecs/qdevice.pri $(QT_INSTALL_DIR)/mkspecs/ || true) && \
			mkdir -p $(ROOT_DIR)/conf/rootfs/fonts && \
			cp -l -f -u `find $(QT_INSTALL_DIR)/lib/fonts/ -type f` $(ROOT_DIR)/conf/rootfs/fonts &&\
			rm -r $(QT_INSTALL_DIR)/include && \
			ln -s $(QT_BUILD_DIR)/include $(QT_INSTALL_DIR)/include; \
		fi

$(ROOTFS_IMAGE): build_and_install

.NOTPARALLEL:
