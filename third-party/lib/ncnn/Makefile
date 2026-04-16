PKG_NAME := ncnn
PKG_VER  := 20250916

PKG_SOURCES := https://github.com/Tencent/$(PKG_NAME)/archive/refs/tags/$(PKG_VER).tar.gz
PKG_MD5     := da74ad0af613a8dd4d9a4a02a234ecef
PKG_PATCHES := patches/0001-embox-platform-template.patch

NCNN_FLAGS = \
	-DCMAKE_TOOLCHAIN_FILE=$(abspath $(MOD_DIR))/cmake/embox-toolchain.cmake \
	-DCMAKE_C_COMPILER:FILEPATH=$(abspath $(EMBOX_GCC)) \
	-DCMAKE_CXX_COMPILER:FILEPATH=$(abspath $(EMBOX_GXX)) \
	-DCMAKE_INSTALL_PREFIX=$(abspath $(PKG_INSTALL_DIR)) \
	-DNCNN_PLATFORM_TEMPLATE=$(abspath $(MOD_DIR))/templates/platform_embox.h.in \
	-DCMAKE_BUILD_TYPE=None \
	-DNCNN_BUILD_TOOLS=OFF \
	-DNCNN_BUILD_EXAMPLES=OFF \
	-DNCNN_BUILD_BENCHMARK=OFF \
	-DNCNN_BUILD_TESTS=OFF \
	-DNCNN_SHARED_LIB=OFF \
	-DNCNN_VULKAN=OFF \
	-DNCNN_OPENMP=OFF \
	-DNCNN_THREADS=OFF \
	-DNCNN_RUNTIME_CPU=OFF \
	-DNCNN_C_API=OFF \
	-DNCNN_BENCHMARK=OFF \
	-DNCNN_STDIO=OFF \
	-DNCNN_STRING=OFF \
	-DNCNN_SIMPLESTL=ON \
	-DNCNN_SIMPLEOMP=OFF \
	-DNCNN_SIMPLEOCV=OFF \
	-DNCNN_AVX=OFF \
	-DNCNN_AVX2=OFF \
	-DNCNN_SSE2=OFF

$(CONFIGURE) :
	mkdir -p $(PKG_SOURCE_DIR)/build-embox

	# Embox-compatible headers for source/build tree
	cp $(MOD_DIR)/features_embox.h $(PKG_SOURCE_DIR)/src/features.h
	cp $(MOD_DIR)/cwchar $(PKG_SOURCE_DIR)/src/cwchar

	cd $(PKG_SOURCE_DIR)/build-embox && cmake .. $(NCNN_FLAGS)

	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR)/build-embox && cmake --build . -j$$(nproc || echo 4)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR)/build-embox && cmake --install .

	# Keep public include tree explicit
	mkdir -p $(PKG_INSTALL_DIR)/include/ncnn
	cp $(MOD_DIR)/platform_embox.h $(PKG_INSTALL_DIR)/include/ncnn/platform.h
	cp $(MOD_DIR)/features_embox.h $(PKG_INSTALL_DIR)/include/ncnn/features.h

	touch $@