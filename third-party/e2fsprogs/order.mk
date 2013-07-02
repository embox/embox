MISC_DIR= $(OBJ_DIR)/third-party/e2fsprogs/../../../../e2fsprogs/src/e2fsprogs-build/misc
EXT2FS_LIB= $(OBJ_DIR)/third-party/e2fsprogs/../../../../e2fsprogs/src/e2fsprogs-build/lib/ext2fs/libext2fs.a

$(MISC_DIR)/e2freefrag: $(EXT2FS_LIB)

$(MISC_DIR)/dumpe2fs: $(EXT2FS_LIB)

$(MISC_DIR)/mke2fs: $(EXT2FS_LIB)

