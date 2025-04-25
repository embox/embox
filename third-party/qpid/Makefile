
PKG_NAME := qpid-cpp
PKG_VER  := 1.39.0

PKG_SOURCES := http://mirror.linux-ia64.org/apache/qpid/cpp/$(PKG_VER)/qpid-cpp-$(PKG_VER).tar.gz
PKG_MD5     := 466ff0b534ee656cbb0e0e33871df31f

PKG_PATCHES := patch.txt \
	fix_boost_warnings.txt

QPID_CXX_FLAGS="-include $(ROOT_DIR)/third-party/qpid/qpid_embox_compat.h \
	-I$(ROOT_DIR)/third-party/qpid/include \
	-I$(ROOT_DIR)/build/extbld/third_party/e2fsprogs/core/install/include\
	-std=gnu++11 \
	-Wno-expansion-to-defined \
	-Wno-write-strings \
	-Wno-unused-parameter"

QPID_LDFLAGS="-L$(ROOT_DIR)/build/extbld/third_party/e2fsprogs/core/install/lib"

QPID_CMAKE_REQUIRED_INCLUDES="$(ROOT_DIR)/build/extbld/third_party/e2fsprogs/core/install/include;$(ROOT_DIR)/third-party/qpid/include"

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	#ln $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/management/Buffer.cpp $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/management/Buffer_.cpp || true
	#ln $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/management/Mutex.cpp  $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/management/Mutex_.cpp || true
	#ln $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/log/Options.cpp       $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/log/Options_.cpp || true
	#ln $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/sys/Shlib.cpp         $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/qpid/sys/Shlib_.cpp || true
	export EMBOX_GCC_LINK=full; \
	export CXXFLAGS=$(QPID_CXX_FLAGS); \
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && rm -rf bld && mkdir bld && cd bld && ( \
		cmake \
		-L \
		-DCMAKE_SYSTEM_NAME:STRING=Generic \
		-DUNIX:BOOL=on \
		-DBUILD_SHARED_LIBS:BOOL=off \
		-DLIB64:STRING=FALSE \
		-DCMAKE_SIZEOF_VOID_P:STRING=4 \
		-DCMAKE_SYSTEM_PROCESSOR:STRING=$(AUTOCONF_TARGET_TRIPLET) \
		-DCMAKE_C_COMPILER:PATH=$(EMBOX_GCC) \
		-DCMAKE_CXX_COMPILER:PATH=$(EMBOX_GXX) \
		-DCMAKE_AR:STRING=ar \
		-DCMAKE_EXE_LINKER_FLAGS:STRING=$(QPID_LDFLAGS) \
		-DBUILD_HA:BOOL=off \
		-DBUILD_SSL:BOOL=off \
		-DBUILD_ACL:BOOL=off \
		-DBUILD_SASL:BOOL=off \
		-DBUILD_XML:BOOL=off \
		-DCMAKE_INSTALL_PREFIX:PATH=$(PKG_INSTALL_DIR)\
		-DCMAKE_BUILD_TYPE:STRING=Debug \
		-DBOOST_ROOT:PATH=$(ROOT_DIR)/build/extbld/third_party/boost/libs \
		-DBOOST_LIBRARYDIR:PATH=$(ROOT_DIR)/build/extbld/third_party/boost/libs/install/lib \
		-DCMAKE_REQUIRED_INCLUDES:PATH=$(QPID_CMAKE_REQUIRED_INCLUDES) \
		-DCMAKE_VERBOSE_MAKEFILE:BOOL=on \
		-DCMAKE_BUILD_TYPE:STRING=Debug \
		-DQPID_LINK_BOOST_DYNAMIC:BOOL=off \
		.. \
		)
	touch $@

$(BUILD) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/bld && ( \
		$(MAKE) qpidd hello_world MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/include $(PKG_INSTALL_DIR)/lib $(PKG_INSTALL_DIR)/bin
	cp -R $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/include/* $(PKG_INSTALL_DIR)/include
	cp $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/bld/src/*.a $(PKG_INSTALL_DIR)/lib
	cp $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/bld/src/CMakeFiles/qpidd.dir/qpidd.o $(PKG_INSTALL_DIR)/bin/qpidd.o
	cp $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/bld/src/CMakeFiles/qpidd.dir/posix/QpiddBroker.o $(PKG_INSTALL_DIR)/bin/QpiddBroker.o
	cp $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/bld/examples/messaging/CMakeFiles/hello_world.dir/hello_world.o $(PKG_INSTALL_DIR)/bin/hello_world.o
	touch $@
