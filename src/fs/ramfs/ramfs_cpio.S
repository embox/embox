#if 0
#ifdef CONFIG_RAMFS_CPIO
.section .ramfs
.incbin CONFIG_RAMFS_CPIO
#endif
#endif
.section .ramfs
.incbin "build/base/rootfs.cpio"

