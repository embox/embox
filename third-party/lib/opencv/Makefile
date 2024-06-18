
define option_get_string
$(shell echo OPTION_STRING_third_party__lib__opencv__build__$(1) | gcc -P -E -include $(SRCGEN_DIR)/include/config/third_party/lib/opencv/build.h -)
endef

define option_get_boolean
$(shell echo OPTION_BOOLEAN_third_party__lib__opencv__build__$(1) | gcc -P -E -include $(SRCGEN_DIR)/include/config/third_party/lib/opencv/build.h -)
endef

PKG_NAME         := opencv
PKG_VER          := $(call option_get_string,opencv_version)
PKG_SOURCES      := https://github.com/opencv/opencv/archive/$(PKG_VER).zip
PKG_ARCHIVE_NAME := $(PKG_NAME)-$(PKG_VER).zip

PKG_MD5_4.5.2    := f100177ab3e7b884f490cba4cff8e074
PKG_MD5_4.0.1    := eb367fc73de66e6d8808e8712138c6b4
PKG_MD5          := $(PKG_MD5_$(PKG_VER))

ifeq ("$(PKG_VER)","4.5.2")
PKG_PATCHES := endian.patch \
               disable_wstring.patch \
               disable_dlopen.patch \
               disable_fpu_cortex_m.patch \
               add_if_defined_embox.patch
ifneq (0,$(call option_get_boolean,hack_4_5_2_disable_fonts))
PKG_PATCHES += disable_hershey_fonts.patch
endif
else ifeq ("$(PKG_VER)","4.0.1")
PKG_PATCHES += patch_4_0_1.txt
else
$(error Unsupported OpenCV version: $(PKG_VER))
endif

include $(EXTBLD_LIB)

OPENCV_FLAGS = \
		-DCMAKE_INSTALL_PREFIX=$(BUILD_DIR)/install \
		-DBUILD_SHARED_LIBS=OFF \
		-DBUILD_opencv_python_bindings_generator=OFF \
		-DBUILD_JAVA=OFF \
		-DBUILD_TESTS=OFF \
		-DBUILD_opencv_apps=OFF \
		-DBUILD_opencv_java_bindings_generator=OFF \
		-DBUILD_opencv_ml=OFF \
		-DWITH_FFMPEG=ON \
		-DFFMPEG_INCLUDE_DIR=$(PKG_INSTALL_DIR)/include \
		-DFFMPEG_LIB_DIR=$(PKG_INSTALL_DIR) \
		-DFFMPEG_LIBRARIES="avcodec;avformat;avutil" \
		-DWITH_EIGEN=OFF \
		-DBUILD_IPP_IW=OFF \
		-DBUILD_ITT=OFF \
		-DBUILD_PERF_TESTS=OFF \
		-DBUILD_PROTOBUF=OFF \
		-DOPENCV_FORCE_3RDPARTY_BUILD=ON \
		-DBUILD_opencv_calib3d=ON \
		-DBUILD_opencv_dnn=ON \
		-DBUILD_opencv_features2d=ON \
		-DBUILD_opencv_flann=ON \
		-DBUILD_opencv_gapi=OFF \
		-DBUILD_opencv_highgui=OFF \
		-DBUILD_opencv_imgcodecs=ON \
		-DBUILD_opencv_imgproc=ON \
		-DBUILD_opencv_java_bindings_generator=OFF \
		-DBUILD_opencv_objdetect=ON \
		-DBUILD_opencv_photo=ON \
		-DBUILD_opencv_stitching=OFF \
		-DBUILD_opencv_ts=ON \
		-DBUILD_opencv_video=ON \
		-DBUILD_opencv_videoio=ON \
		-DBUILD_opencv_js_bindings_generator=OFF \
		-DBUILD_opencv_objc_bindings_generator=OFF \
		-DCPACK_BINARY_STGZ=OFF \
		-DCPACK_BINARY_TGZ=OFF \
		-DCPACK_BINARY_TZ=OFF \
		-DCPACK_SOURCE_TBZ2=OFF \
		-DCPACK_SOURCE_TGZ=OFF \
		-DCPACK_SOURCE_TXZ=OFF \
		-DCPACK_SOURCE_TZ=OFF \
		-DCV_ENABLE_INTRINSICS=OFF \
		-DCV_TRACE=OFF \
		-DENABLE_CCACHE=OFF \
		-DENABLE_OMIT_FRAME_POINTER=OFF \
		-DENABLE_PIC=OFF \
		-DENABLE_PRECOMPILED_HEADERS=OFF \
		-DOPENCL_FOUND=OFF \
		-DOPENCV_DNN_OPENCL=OFF \
		-DOPENCV_GENERATE_SETUPVARS=OFF \
		-DVIDEOIO_ENABLE_PLUGINS=ON \
		-DVIDEOIO_ENABLE_STRICT_PLUGIN_CHECK=ON \
		-DWITH_1394=OFF \
		-DWITH_CAROTENE=OFF \
		-DWITH_ADE=OFF \
		-DWITH_GSTREAMER=OFF \
		-DWITH_GTK=OFF \
		-DWITH_IMGCODEC_HDR=ON \
		-DWITH_IMGCODEC_PFM=ON \
		-DWITH_IMGCODEC_PXM=ON \
		-DWITH_IMGCODEC_SUNRASTER=OFF \
		-DWITH_IPP=OFF \
		-DWITH_ITT=OFF \
		-DWITH_JASPER=OFF \
		-DWITH_JPEG=OFF \
		-DWITH_OPENJPEG=OFF \
		-DWITH_LAPACK=OFF \
		-DWITH_OPENCL=OFF \
		-DWITH_OPENCLAMDBLAS=OFF \
		-DWITH_OPENCLAMDFFT=OFF \
		-DWITH_OPENEXR=OFF \
		-DWITH_PNG=ON \
		-DWITH_PROTOBUF=OFF \
		-DWITH_PTHREADS_PF=OFF \
		-DWITH_QUIRC=OFF \
		-DWITH_TIFF=OFF \
		-DWITH_V4L=OFF \
		-DWITH_VTK=OFF \
		-DWITH_WEBP=OFF \
		-DPARALLEL_ENABLE_PLUGINS=OFF \
		-DCMAKE_SYSTEM_PROCESSOR:STRING=$(AUTOCONF_TARGET_TRIPLET) \
		-DCMAKE_C_COMPILER:PATH=$(EMBOX_GCC) \
		-DCMAKE_CXX_COMPILER:PATH=$(EMBOX_GXX) \
		-DCMAKE_AR:STRING=ar \
		-DCMAKE_SYSTEM_NAME:STRING=Generic \
		-DCMAKE_BUILD_TYPE:STRING=None \
		-DCMAKE_VERBOSE_MAKEFILE:BOOL=on \
		-DC_HAS_MFPU_NEON=0

# FIXME mv $(PKG_NAME)-$(PKG_VER)/modules/core/src/parallel/parallel.cpp is used
# becase there are two files in modules/core/src/ with the same name parallel.cpp which
# then leads to broken gnu ar extraction.
$(BUILD) :
	cd $(BUILD_DIR) && mkdir build; \
	mv $(PKG_NAME)-$(PKG_VER)/modules/core/src/parallel/parallel.cpp $(PKG_NAME)-$(PKG_VER)/modules/core/src/parallel/parallel2.cpp; \
	cd build && cmake $(OPENCV_FLAGS) ../$(PKG_NAME)-$(PKG_VER)/ && \
	$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'
	touch $@

$(INSTALL) :
	cd $(BUILD_DIR)/build && make install
	touch $@
