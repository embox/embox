
PKG_NAME := ioq3
PKG_VER  :=

PKG_SOURCES := https://www.dropbox.com/s/p95hf32zmuc53tg/ioq3.tar.gz
PKG_MD5     := c1a756ff87bc96bb33e99980f330f11a

PKG_PATCHES := patch.txt
include $(EXTBLD_LIB)

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -mno-unaligned-access,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/src/include
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/src/compat/posix/include
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/src/compat/libc/include
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/src/compat/linux/include
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/third-party/games/quake3
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/build/base/gen/include/
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/./src/arch/x86/include/
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/src/net/lib
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/./build/extbld/third_party/games/quake3/ioq3/code/
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/build/extbld/third_party/games/quake3/ioq3/code/curl-7.54.0/include/
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/build/extbld/third_party/games/quake3/ioq3/./code/SDL2/include/
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/./build/extbld/third_party/lib/sdl2/SDL2-2.0.8/include/
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/./build/extbld/third_party/lib/OpenLibm/openlibm-0.7.0/src/
EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/build/extbld/third_party/lib/OpenLibm/openlibm-0.7.0/include/

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/ioq3 && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' V=1 CFLAGS="-g $(EMBOX_IMPORTED_CFLAGS) -nostdinc"; \
	)
	touch $@
