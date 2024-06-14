PKG_NAME := ffmpeg
PKG_VER  := 4.3.1

PKG_SOURCE_DIR := $(EXTERNAL_BUILD_DIR)/third_party/ffmpeg/$(PKG_NAME)-$(PKG_VER)

PKG_SOURCES := https://ffmpeg.org/releases/$(PKG_NAME)-$(PKG_VER).tar.bz2
PKG_MD5	:= 804707549590e90880e8ecd4e5244fd8

include $(EXTBLD_LIB)

FF_CPPFLAGS := -I$(ROOT_DIR)/third-party/lib/ffmpeg/include
FF_CPPFLAGS += -I$(SRC_DIR)/include
FF_CPPFLAGS += -I$(SRC_DIR)/compat
FF_CPPFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/gcc/include
FF_CPPFLAGS += -I$(SRC_DIR)/compat/libc/include
FF_CPPFLAGS += -I$(SRC_DIR)/compat/cxx/include
FF_CPPFLAGS += -I$(SRC_DIR)/compat/posix/include
FF_CPPFLAGS += -I$(SRC_DIR)/arch/$(ARCH)/include 
FF_CPPFLAGS += -I$(ROOT_DIR)/build/base/gen/include

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --prefix=$(PKG_INSTALL_DIR) \
			--cc=$(EMBOX_GCC) \
			--cxx=$(EMBOX_GXX) \
			--target-os=none \
			--disable-asm \
			--disable-programs \
			--disable-doc \
			--disable-avdevice \
			--disable-swscale \
			--disable-postproc \
			--disable-avfilter \
			--disable-network \
			--disable-hwaccels \
			--disable-everything \
			--enable-cross-compile \
			--enable-gpl \
			--enable-nonfree \
			--enable-protocol=file \
			--enable-decoder=h264 \
			--enable-parser=h264 \
			--enable-demuxer=h264 \
			--enable-decoder=mpeg4 \
			--enable-parser=mpeg4video \
			--enable-demuxer=mov \
			--enable-demuxer=avi \
			--enable-static \
			--disable-shared \
			--pkgconfigdir=$(PKG_INSTALL_DIR) \
			--disable-runtime-cpudetect \
			--disable-protocols \
			--disable-devices \
			--disable-lzma \
			--disable-bzlib \
			--disable-xlib \
			--disable-zlib \
			--disable-iconv \
			--disable-libxcb \
			--disable-libxcb-shm \
			--disable-libxcb-xfixes \
			--disable-libxcb-shape \
			--disable-sdl2 \
			--extra-cflags="$(FF_CPPFLAGS)" \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
		cp libavcodec/libavcodec.a $(PKG_INSTALL_DIR)/; \
		cp libavformat/libavformat.a $(PKG_INSTALL_DIR)/; \
		cp libavutil/libavutil.a $(PKG_INSTALL_DIR)/; \
	)
	touch $@
