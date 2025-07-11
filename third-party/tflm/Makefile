PKG_NAME        := tflite-micro
PKG_VER         := main

PKG_SOURCES     := git+https://github.com/tensorflow/tflite-micro.git
PKG_SOURCE_DIR  := $(PKG_DOWNLOAD_DIR)/$(PKG_NAME)


TFLM_CPPFLAGS  += -I$(PKG_SOURCE_DIR)
TFLM_CPPFLAGS  += -I$(PKG_SOURCE_DIR)/tensorflow
TFLM_CPPFLAGS  += -I$(PKG_SOURCE_DIR)/tensorflow/lite/micro
TFLM_CPPFLAGS  += -I$(PKG_SOURCE_DIR)/third_party/gemmlowp
TFLM_CPPFLAGS  += -I$(PKG_SOURCE_DIR)/third_party/flatbuffers/include


$(CONFIGURE): 
	touch $@


$(BUILD):
	touch $@


$(INSTALL):
	mkdir -p $(PKG_INSTALL_DIR)/include/tflm
	cp -r $(PKG_SOURCE_DIR)/tensorflow $(PKG_INSTALL_DIR)/include/tflm/
	touch $@
