
PKG_NAME := qt-everywhere-opensource-src
PKG_VER  := 4.8.5

PKG_SOURCES := http://download.qt-project.org/archive/qt/4.8/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 1864987bdbb2f58f8ae8b350dfdbe133

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

define option_get
$(shell echo OPTION_BOOLEAN_third_party__qt__core__$(1) | gcc -P -E -include $(SRCGEN_DIR)/include/config/third_party/qt/core.h -)
endef

QT_CONF_FLAGS := -hostprefix $(PKG_INSTALL_DIR)
ifeq (arm,$(EMBOX_ARCH))
QT_CONF_FLAGS += -arch arm -little-endian
else ifeq (x86,$(EMBOX_ARCH))
QT_CONF_FLAGS += -arch i386 -little-endian
else ifeq (microblaze,$(EMBOX_ARCH))
QT_CONF_FLAGS += -arch microblaze -big-endian
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif
ifneq (,$(CROSS_COMPILE))
QT_CONF_FLAGS += -device-option CROSS_COMPILE='$(CROSS_COMPILE)'
endif
QT_CONF_FLAGS += -embedded-lite vnc -v
QT_CONF_FLAGS += -xplatform qws/embox-generic-g++ -static
QT_CONF_FLAGS += -opensource -confirm-license
QT_CONF_FLAGS += -no-pch -fast
QT_CONF_FLAGS += -no-largefile -no-qt3support
QT_CONF_FLAGS += -no-kbd-tty -no-mouse-pc -no-mouse-linuxtp \
				 -no-gfx-linuxfb -no-gfx-multiscreen
QT_CONF_FLAGS += -no-feature-TEMPORARYFILE -D QT_NO_TEMPORARYFILE
ifneq (0,$(call option_get,process_support))
QT_CONF_FLAGS += -feature-PROCESS
else
QT_CONF_FLAGS += -no-feature-PROCESS -D QT_NO_PROCESS
endif
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

QMAKE_CONF_M4 := $(PKG_SOURCE_DIR)/$(shell (cd tree && find . -name qmake.conf.m4))
QMAKE_CONF    := $(patsubst %.m4,%,$(QMAKE_CONF_M4))
ifneq ($(words $(QMAKE_CONF_M4)),1)
$(error need exactly one qmake.conf.m4)
endif

QT_CPPFLAGS := $(filter -I%,$(EMBOX_CPPFLAGS))
QT_CPPFLAGS += -include qt_embox_compat.h
ifeq (1,$(call option_get,stl_support))
QT_CPPFLAGS += -I$(EXTERNAL_BUILD_DIR)/third_party/STLport/core/include/stlport
endif

QT_CFLAGS   := $(QT_CPPFLAGS) -Wno-error
QT_CXXFLAGS := $(QT_CPPFLAGS) $(filter -I%,$(EMBOX_CXXFLAGS)) -fpermissive -Wno-error
QT_CXXFLAGS := $(subst -I./,-I$(ROOT_DIR)/,$(QT_CXXFLAGS))

QT_MAKEFLAGS=
ifneq ($(filter -j, $(EMBOX_MAKEFLAGS)),)
	QT_MAKEFLAGS += -j $(shell nproc)
endif

$(CONFIGURE) :
	cd $(PKG_SOURCE_DIR) && ( \
		CC='$(EMBOX_GCC)' CXX='$(EMBOX_GXX)' CFLAGS='$(QT_CFLAGS)' CXXFLAGS='$(QT_CXXFLAGS)' \
			m4 --prefix-builtins $(QMAKE_CONF_M4) > $(QMAKE_CONF) && \
		MAKEFLAGS='$(QT_MAKEFLAGS)' ./configure $(QT_CONF_FLAGS) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(QT_MAKEFLAGS)'; \
	)
	touch $@

$(ROOTFS_IMAGE) : $(INSTALL)

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install MAKEFLAGS='$(QT_MAKEFLAGS)' \
	)
	cp $(PKG_SOURCE_DIR)/mkspecs/qdevice.pri $(PKG_INSTALL_DIR)/mkspecs/
	mkdir -p $(ROOT_DIR)/conf/rootfs/fonts
	cp -l -f -u `find $(PKG_INSTALL_DIR)/lib/fonts/ -type f` $(ROOT_DIR)/conf/rootfs/fonts
	rm -r $(PKG_INSTALL_DIR)/include
	ln -s $(PKG_SOURCE_DIR)/include $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/.qmake.cache $(PKG_INSTALL_DIR)
	touch $@
