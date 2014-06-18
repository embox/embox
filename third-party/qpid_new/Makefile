
PKG_NAME := qpidc
PKG_VER  := 0.24

PKG_SOURCES := http://archive.apache.org/dist/qpid/0.24/qpid-cpp-$(PKG_VER).tar.gz
PKG_MD5     := f6245e2e68ccd1bac8fe706db3fba707

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

samba_cppflags = -include $(ROOT_DIR)/third-party/qpid_new/qpid_embox_compat.h \
	-I$(ROOT_DIR)/third-party/qpid_new/include \
	${EMBOX_IMPORTED_CPPFLAGS}

$(CONFIGURE) :
	ln $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/management/Buffer.cpp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/management/Buffer_.cpp
	ln $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/management/Mutex.cpp  $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/management/Mutex_.cpp
	ln $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/log/Options.cpp       $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/log/Options_.cpp
	ln $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/sys/Shlib.cpp         $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/sys/Shlib_.cpp
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && rm -rf bld && mkdir bld && cd bld && ( \
		cmake \
		-DBoost_DEBUG:BOOL=on \
		-DCMAKE_SYSTEM_NAME:STRING=Generic \
		-DUNIX:BOOL=on \
		-DLIB64:STRING=FALSE \
		-DCMAKE_SIZEOF_VOID_P:STRING=4 \
		-DCMAKE_SYSTEM_PROCESSOR:STRING=$(AUTOCONF_TARGET_TRIPLET) \
		-DCMAKE_C_COMPILER:PATH=$(EMBOX_GCC) \
		-DCMAKE_CXX_COMPILER:PATH=$(EMBOX_GXX) \
		-DBUILD_HA:BOOL=off \
		-DBUILD_SSL:BOOL=off \
		-DCMAKE_INSTALL_PREFIX:PATH=$(PKG_INSTALL_DIR)\
		-DCMAKE_BUILD_TYPE:STRING=Debug \
		-DBOOST_ROOT:PATH=$(ROOT_DIR)/build/extbld/third_party/boost/core \
		-DCMAKE_REQUIRED_INCLUDES:PATH=$(ROOT_DIR)/build/extbld/third_party/e2fsprogs/core/include \
		.. \
		)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/bld && ( \
		make all MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	echo "INSTALL IS NOT SUPPORTED!"
	exit 1
	touch $@
