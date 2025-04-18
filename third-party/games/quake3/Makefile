
PKG_NAME := ioq3
PKG_VER  :=

PKG_SOURCES := https://www.dropbox.com/s/p95hf32zmuc53tg/ioq3.tar.gz
PKG_MD5     := c1a756ff87bc96bb33e99980f330f11a

PKG_PATCHES := patch.txt \
			q_platform_patch.txt \
			sys_patch.txt

#PKG_PATCHES += video_mode_patch.txt

PKG_PATCHES += sse_patch.txt

include $(EXTBLD_TARGETS)

QUAKE_EMBOX_CFLAGS := -include $(ROOT_DIR)/third-party/games/quake3/embox_compat.h
QUAKE_EMBOX_CFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/lib/OpenLibm/openlibm-0.7.0/include/
QUAKE_EMBOX_CFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/lib/OpenLibm/openlibm-0.7.0/src/
QUAKE_EMBOX_CFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/games/quake3/ioq3/./code/SDL2/include/
QUAKE_EMBOX_CFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/lib/sdl2/SDL2-2.0.8/include/
QUAKE_EMBOX_CFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/lib/curl/curl-7.61.1/include/

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/ioq3 && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' V=1 ARCH=$(EMBOX_ARCH) CC=$(EMBOX_GCC) CFLAGS="$(QUAKE_EMBOX_CFLAGS)"; \
	)
	touch $@
