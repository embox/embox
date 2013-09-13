MAKE_E2FS = \
	$(MAKE) -C $(THIRDPARTY_DIR)/e2fsprogs \
	MAKEFLAGS= \
	EMBOX_MAKEFLAGS='$(MAKEFLAGS)' \
	ROOT_DIR=$(abspath $(ROOT_DIR)) \
	EMBOX_CFLAGS='$(CFLAGS)' \
	EMBOX_CXXFLAGS='$(CXXFLAGS)' \
	EMBOX_LDFLAGS='$(LDFLAGS)' \
	EMBOX_CPPFLAGS='$(EMBOX_EXPORT_CPPFLAGS)'

MISC_DIR= $(OBJ_DIR)/third-party/e2fsprogs/../../../../e2fsprogs/src/e2fsprogs-build/misc
E2FSCK_DIR= $(OBJ_DIR)/third-party/e2fsprogs/../../../../e2fsprogs/src/e2fsprogs-build/e2fsck
EXT2FS_LIB= $(OBJ_DIR)/third-party/e2fsprogs/../../../../e2fsprogs/src/e2fsprogs-build/lib/ext2fs/libext2fs.a

$(MISC_DIR)/e2freefrag: $(EXT2FS_LIB)

$(MISC_DIR)/dumpe2fs: $(EXT2FS_LIB)

$(MISC_DIR)/mke2fs: $(EXT2FS_LIB)

$(E2FSCK_DIR)/e2fsck: $(EXT2FS_LIB)
