PKG_NAME := tflite-micro
PKG_VER  := 2b3f8d0f018bc9911fc134682e08fec05d851118

PKG_SOURCES := https://github.com/tensorflow/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5 := bd25104002ef1f7ab95f6aca7a821bb3

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(MKDIR) $(PKG_INSTALL_DIR)/src ;
	cp -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR)/src/ ;
	touch $@
