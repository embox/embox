PKG_NAME := boost

# PKG_VER  := 1_85_0
# PKG_SOURCES := https://boostorg.jfrog.io/artifactory/main/release/1.85.0/source/boost_$(PKG_VER).tar.gz
# PKG_MD5     := 53aeccc3167909ee770e34469f8dd592

PKG_VER  := 1_65_0
PKG_SOURCES := https://boostorg.jfrog.io/artifactory/main/release/1.65.0/source/boost_$(PKG_VER).tar.gz
PKG_MD5     := d75a27ad1a34cfc7c9b5cb92594410c3

PKG_PATCHES := patch.txt

PKG_SOURCE_DIR = $(BUILD_DIR)/$(PKG_NAME)_$(PKG_VER)

include $(EXTBLD_LIB)

BOOST_ADDITIONAL_CPPFLAGS += -I$(abspath .)/include
BOOST_ADDITIONAL_CPPFLAGS += -include $(abspath .)/boost_embox_compat.h

USER_TOOLS_CONFIG=$(PKG_SOURCE_DIR)/tools/build/src/user-config.jam
USER_BOOST_CONFIG=$(PKG_SOURCE_DIR)/boost/config/user.hpp

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./bootstrap.sh \
		--prefix=$(PKG_INSTALL_DIR) \
		--with-libraries=atomic,chrono,context,date_time,exception,program_options,random,system,thread,timer \
	)
	touch $@

B2_OPTIONS=-q \
	cxxflags="$(BOOST_ADDITIONAL_CPPFLAGS) -Wno-narrowing" \
	target-os=unix \
	variant=debug \
	link=static \
	threading=multi \
	address-model=32 \
	binary-format=elf \
	architecture=$(EMBOX_ARCH) \
	toolset=gcc \
	define=BOOST_HAS_SCHED_YIELD \
	define=BOOST_HAS_GETTIMEOFDAY \
	define=BOOST_NO_INTRINSIC_WCHAR_T \
	define=BOOST_NO_CXX11_AUTO_DECLARATIONS \
	define=BOOST_MATH_DISABLE_FLOAT128

$(BUILD) :
	touch $(USER_TOOLS_CONFIG)
	echo "using gcc : : $(EMBOX_GXX) ;" > $(USER_TOOLS_CONFIG)
	echo "#define BOOST_HAS_THREADS" >> $(USER_BOOST_CONFIG)
	echo "#define BOOST_HAS_PTHREADS" >> $(USER_BOOST_CONFIG)
	echo "#define BOOST_STRICT_CONFIG" >> $(USER_BOOST_CONFIG)
	echo "#define BOOST_MATH_DISABLE_FLOAT128" >> $(USER_BOOST_CONFIG)
	# Build boost libraries
	cd $(PKG_SOURCE_DIR) && ( \
		./b2 \
		$(B2_OPTIONS) \
		install \
	)
	# Build example
	cd $(PKG_SOURCE_DIR) && ( \
		./b2 \
		$(B2_OPTIONS) \
		libs/date_time/example time_math \
	)
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/examples
	cp $(PKG_SOURCE_DIR)/$(shell cd $(PKG_SOURCE_DIR); find . -name time_math.o) $(PKG_INSTALL_DIR)/examples
	touch $@
