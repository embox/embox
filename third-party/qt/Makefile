
include $(EXTBLD_DEFINES)

PKG_NAME := qt-everywhere-opensource-src
PKG_VER  := 4.8.7

# PKG_SOURCES := http://download.qt-project.org/archive/qt/4.8/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz \
# 		http://download.embox.rocks/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_SOURCES := https://github.com/rochus-keller/Qt-4.8.7/archive/refs/heads/master.zip

# PKG_MD5 := d990ee66bf7ab0c785589776f35ba6ad
PKG_MD5 := d8eaa1385eec9b7a1cdc0ed3599ab179

#
# NOTE: q_foreach_bug_g++9.txt patch is required because
# QT 4 has a buggy Q_FOREACH() macro which breaks "for" loop
# at incorrect place (outer loop). It is discussed here
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=44715,
# and here
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90617
#
# QT 5 has a better version of Q_FOREACH(), so this patch
# derives this improved version from QT 5 sources.
#

PKG_PATCHES := pkg_patch.txt \
	patch_cortex_m7.txt \
	q_foreach_bug_g++9.txt \
	animatedtiles_fullscreen_option.txt \
	fb_base_touchscreen_cursor.txt

ifeq ($(call option_get,STRING,qconfig),small)
PKG_PATCHES += pkg_patch_for_qconfig.txt
PKG_PATCHES += qconfig-small.txt
endif

include $(EXTBLD_TARGETS)

QT_CONF_FLAGS := -hostprefix $(PKG_INSTALL_DIR)
ifeq (arm,$(EMBOX_ARCH))
QT_CONF_FLAGS += -arch arm -little-endian
QT_CONF_FLAGS += -D QT_NO_ARM_EABI
else ifeq (x86,$(EMBOX_ARCH))
QT_CONF_FLAGS += -arch i386 -little-endian
else ifeq (microblaze,$(EMBOX_ARCH))
QT_CONF_FLAGS += -arch microblaze -big-endian
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif
ifneq (,$(EMBOX_CROSS_COMPILE))
QT_CONF_FLAGS += -device-option CROSS_COMPILE='$(EMBOX_CROSS_COMPILE)'
endif
QT_CONF_FLAGS += -embedded-lite -v
QT_CONF_FLAGS += -xplatform qws/embox-generic-g++ -static
QT_CONF_FLAGS += -opensource -confirm-license
QT_CONF_FLAGS += -no-pch -fast
QT_CONF_FLAGS += -no-largefile -no-qt3support
QT_CONF_FLAGS += -no-kbd-tty -no-mouse-pc -no-mouse-linuxtp \
				 -no-gfx-linuxfb -no-gfx-multiscreen
QT_CONF_FLAGS += -no-feature-TEMPORARYFILE -D QT_NO_TEMPORARYFILE
ifneq (0,$(call option_get,BOOLEAN,process_support))
QT_CONF_FLAGS += -feature-PROCESS
else
QT_CONF_FLAGS += -no-feature-PROCESS -D QT_NO_PROCESS
endif
QT_CONF_FLAGS += -no-feature-SHAREDMEMORY -D QT_NO_SHAREDMEMORY
QT_CONF_FLAGS += -no-feature-NETWORKINTERFACE -D QT_NO_NETWORKINTERFACE
QT_CONF_FLAGS += -no-feature-HOSTINFO -D QT_NO_HOSTINFO
QT_CONF_FLAGS += -no-feature-QWS_INPUTMETHODS -D QT_NO_QWS_INPUTMETHODS
ifneq (0,$(call option_get,BOOLEAN,stl_support))
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
QT_CONF_FLAGS += -no-freetype
QT_CONF_FLAGS += -no-xmlpatterns

ifneq ($(call option_get,BOOLEAN,vnc_support),0)
QT_CONF_FLAGS += -qt-gfx-vnc
endif

ifneq ($(call option_get,BOOLEAN,touchscreen),0)
QT_CONF_FLAGS += -D QT_EMBOX_TOUCHSCREEN
endif

ifeq ($(call option_get,STRING,qconfig),small)
QT_CONF_FLAGS += -qconfig small
QT_CONF_FLAGS += -D QT_NO_NETWORKPROXY
endif

QMAKE_CONF_M4 := $(PKG_SOURCE_DIR)/$(shell (cd tree && find . -name qmake.conf.m4))
QMAKE_CONF    := $(patsubst %.m4,%,$(QMAKE_CONF_M4))
ifneq ($(words $(QMAKE_CONF_M4)),1)
$(error need exactly one qmake.conf.m4)
endif

QT_CPPFLAGS += -include qt_embox_compat.h
QT_CFLAGS   := $(QT_CPPFLAGS) -Wno-error
QT_CXXFLAGS := $(QT_CPPFLAGS) -fpermissive -Wno-error

$(CONFIGURE) :
	cd $(PKG_SOURCE_DIR) && ( \
		CC='$(EMBOX_GCC)' CXX='$(EMBOX_GXX)' CFLAGS='$(QT_CFLAGS)' CXXFLAGS='$(QT_CXXFLAGS)' \
			m4 --prefix-builtins $(QMAKE_CONF_M4) > $(QMAKE_CONF) && \
		MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' ./configure $(QT_CONF_FLAGS) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(ROOTFS_IMAGE) : $(INSTALL)

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
	)
	cp $(PKG_SOURCE_DIR)/mkspecs/qdevice.pri $(PKG_INSTALL_DIR)/mkspecs/
	mkdir -p $(ROOT_DIR)/conf/rootfs/fonts
ifneq ($(call option_get,BOOLEAN,fonts_support),0)
	cp -l -f -u `find $(PKG_INSTALL_DIR)/lib/fonts/ -type f` $(ROOT_DIR)/conf/rootfs/fonts
endif
	rm -r $(PKG_INSTALL_DIR)/include
	ln -s $(PKG_SOURCE_DIR)/include $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/.qmake.cache $(PKG_INSTALL_DIR)
	touch $@
