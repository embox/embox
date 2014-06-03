
PKG_NAME := samba
PKG_VER  := 3.6.18

PKG_SOURCES := http://ftp.samba.org/pub/samba/stable/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := c7eec3e83fe4c4750240a8a0a214bbd4

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

samba_cppflags = -include $(ROOT_DIR)/third-party/samba_new/samba_embox_compat.h \
	-I$(ROOT_DIR)/third-party/samba_new/include \
	-I$(ROOT_DIR)/build/extbld/third_party/zlib/libs/zlib-1.2.8 \
	${EMBOX_IMPORTED_CPPFLAGS}

$(CONFIGURE) :
	cp $(ROOT_DIR)/third-party/samba_new/tree/source3/runfromhere.sh $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		source3/runfromhere.sh ./configure --prefix=$(PKG_INSTALL_DIR) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="$(samba_cppflags)" \
			PKG_CONFIG=/dev/null \
			--with-libiconv=$(PKG_INSTALL_DIR)\
			--enable-debug \
			--enable-developer \
			--disable-largefile \
			--disable-pie \
			--enable-static=ALL \
			--disable-shared \
			--disable-shared-libs \
	      libreplace_cv_HAVE_GETADDRINFO=no \
	      ac_cv_file__proc_sys_kernel_core_pattern=no \
	      ac_cv_func_poll=yes \
	      libreplace_cv_HAVE_MMAP=no \
	      ac_cv_lib_readline_rl_callback_handler_install=no \
	      ac_cv_func_getpassphrase=no \
	      ac_cv_func_sysctlbyname=no \
	      ac_cv_func_printf=yes \
	      ac_cv_func_pread=no \
	      ac_cv_func_pwrite=no \
	      ac_cv_func_pread64=no \
	      ac_cv_func_pwrite64=no \
	      ac_cv_func_posix_fallocate=no \
	      ac_cv_func_posix_fallocate64=no \
	      ac_cv_func_fdopendir=no \
	      ac_cv_func_memcpy=yes \
	      ac_cv_func_memset=yes \
	      ac_cv_func_mknod=no \
	      ac_cv_func_mknod64=no \
	      ac_cv_func_setgroups=no \
	      ac_cv_func_syslog=no \
	      ac_cv_func_strerror=yes \
	      ac_cv_func_lgetxattr=no \
	      ac_cv_func_lgetea=no \
	      ac_cv_func_attr_get=no \
	      ac_cv_func_llistxattr=no \
	      ac_cv_func_llistea=no \
	      ac_cv_func_removexattr=no \
	      ac_cv_func_removeea=no \
	      ac_cv_func_extattr_delete_file=no \
	      ac_cv_func_attr_remove=no \
	      ac_cv_func_lremovexattr=no \
	      ac_cv_func_lremoveea=no \
	      ac_cv_func_fremovexattr=no \
	      ac_cv_func_fremoveea=no \
	      ac_cv_func_attr_removef=no \
	      ac_cv_func_lsetxattr=no \
	      ac_cv_func_lsetea=no \
	      ac_cv_func_attr_set=no \
	      ac_cv_func_yp_get_default_domain=no \
	      ac_cv_func_fchmod=no \
	      ac_cv_func_fdatasync=no \
	      ac_cv_lib_rt_fdatasync=no \
	      ac_cv_func_posix_memalign=no \
	      ac_cv_func_getpeereid=no \
	      samba_cv_HAVE_GETPEEREID=no \
	      ac_cv_func_setsid=no \
	      fu_cv_sys_stat_statvfs=no \
	      ac_cv_func_dlopen=no \
	      ac_cv_func_shl_load=no \
	      ac_cv_func_shl_unload=no \
	      ac_cv_func_shl_findsym=no \
	      ac_cv_func_utime=no \
	      ac_cv_func_getpwent_r=no \
	      ac_cv_func_getgrent_r=no \
	      ac_cv_func_getgrouplist=no \
	      ac_cv_func_sigblock=no \
	      ac_cv_func_sigaction=yes \
	      ac_cv_func_getgrset=no \
	      ac_cv_func_nanosleep=no \
	      ac_cv_lib_ext_rt_nanosleep=no \
	      ac_cv_search_backtrace_symbols=no \
	      ac_cv_func_backtrace_symbols=no \
	      ac_cv_func_connect=yes \
	      ac_cv_func_strlcpy=no \
	      ac_cv_func_strndup=no \
	      ac_cv_func_strnlen=no \
	      ac_cv_func_waitpid=yes \
	      ac_cv_func_strtoull=yes \
	      ac_cv_func_getpass=yes \
	      libreplace_cv_REPLACE_GETPASS=no \
	      ac_cv_header_zlib_h=yes \
		  ac_cv_lib_ext_z_zlibVersion=yes \
		  samba_cv_zlib_1_2_3=yes \
		)
	touch $@

$(BUILD) :
	export EXEEXT=.embox.o; \
	export LIBS=libsmbclient bin/smbclient.embox.o; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)  && ( \
		source3/runfromhere.sh make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	echo "SAMBA INSTALL NOT SUPPORTED"
	exit 1
	touch $@
