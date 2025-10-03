
define option_get_with_ffmpeg
$(shell echo OPTION_BOOLEAN_third_party__lib__opencv__videoio_backend__with_ffmpeg | \
gcc -I$(SRCGEN_DIR)/include -P -E -include $(SRCGEN_DIR)/include/config/third_party/lib/opencv/videoio_backend.h -)
endef

PKG_NAME         := opencv
PKG_VER          := $(call option_get,STRING,opencv_version)
PKG_SOURCES      := https://github.com/opencv/opencv/archive/$(PKG_VER).zip

PKG_MD5_4.5.2    := f100177ab3e7b884f490cba4cff8e074
PKG_MD5_4.0.1    := eb367fc73de66e6d8808e8712138c6b4
PKG_MD5          := $(PKG_MD5_$(PKG_VER))

ifeq ("$(PKG_VER)","4.5.2")
PKG_PATCHES := patch_4_5_2/disable_wstring.patch \
               patch_4_5_2/disable_dlopen.patch \
               patch_4_5_2/add_if_defined_embox.patch

ifeq (1,$(call macro_get,__thumb__))
PKG_PATCHES += patch_4_5_2/disable_fpu_cortex_m.patch
endif

ifeq (1,$(call option_get,BOOLEAN,hack_4_5_2_disable_fonts))
PKG_PATCHES += patch_4_5_2/disable_hershey_fonts.patch
endif

ifeq (1,$(call option_get_with_ffmpeg))
PKG_PATCHES += patch_4_5_2/with_ffmpeg.patch
endif

else ifeq ("$(PKG_VER)","4.0.1")
PKG_PATCHES += patch_4_0_1.txt
else
$(error Unsupported OpenCV version: $(PKG_VER))
endif

ifeq (1,$(call option_get_with_ffmpeg))
WITH_FFMPEG := ON
else
WITH_FFMPEG := OFF
endif

ifeq (1,$(call macro_get,__ARM_NEON__))
INTRINSIC_FLAGS := -DCV_ENABLE_INTRINSICS=ON -DENABLE_NEON=ON
else ifeq (1,$(call macro_get,__SSE2__))
INTRINSIC_FLAGS := -DCV_ENABLE_INTRINSICS=ON -DENABLE_SSE2=ON
else
INTRINSIC_FLAGS := -DCV_ENABLE_INTRINSICS=OFF
endif

CV_LOG_LEVEL := $(call option_get,NUMBER,cv_log_level)
CV_CXX_FLAGS := -DCV_LOG_STRIP_LEVEL=$(CV_LOG_LEVEL)

OPENCV_FLAGS = \
		-DCMAKE_INSTALL_PREFIX=$(MOD_BUILD_DIR)/install \
		-DBUILD_SHARED_LIBS=OFF \
		-DBUILD_opencv_python_bindings_generator=OFF \
		-DBUILD_JAVA=OFF \
		-DBUILD_TESTS=OFF \
		-DBUILD_opencv_apps=OFF \
		-DBUILD_opencv_java_bindings_generator=OFF \
		-DBUILD_opencv_ml=OFF \
		-DWITH_FFMPEG=$(WITH_FFMPEG) \
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
		$(INTRINSIC_FLAGS) \
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
		-DCMAKE_AR:STRING=$(EMBOX_CROSS_COMPILE)ar \
		-DCMAKE_CXX_FLAGS:STRING="$(CV_CXX_FLAGS)" \
		-DCMAKE_SYSTEM_NAME:STRING=Generic \
		-DCMAKE_BUILD_TYPE:STRING=None \
		-DCMAKE_VERBOSE_MAKEFILE:BOOL=on

OPENCV_BUILD_DIR := $(MOD_BUILD_DIR)/build

# FIXME mv $(PKG_NAME)-$(PKG_VER)/modules/core/src/parallel/parallel.cpp is used
# becase there are two files in modules/core/src/ with the same name parallel.cpp which
# then leads to broken gnu ar extraction.
$(CONFIGURE) :
	$(MV) $(PKG_SOURCE_DIR)/modules/core/src/parallel/parallel.cpp \
		$(PKG_SOURCE_DIR)/modules/core/src/parallel/parallel2.cpp; \
	$(MKDIR) $(OPENCV_BUILD_DIR)
	export EMBOX_GCC_LINK=full; \
	cd $(OPENCV_BUILD_DIR) && ( \
		cmake $(OPENCV_FLAGS) $(PKG_SOURCE_DIR); \
	)
	touch $@

$(BUILD) :
	cd $(OPENCV_BUILD_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(OPENCV_BUILD_DIR) && ( \
		$(MAKE) install; \
	)
	touch $@
