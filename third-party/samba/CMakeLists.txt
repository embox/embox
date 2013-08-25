include(ExternalProject)
ExternalProject_Add(
   samba
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://ftp.samba.org/pub/samba/samba-3.6.18.tar.gz
   URL_MD5 c7eec3e83fe4c4750240a8a0a214bbd4
   UPDATE_COMMAND ""
   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/samba/tree &&
      bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`" &&
      patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/samba/patch.txt
   BUILD_IN_SOURCE 1
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
   	  <SOURCE_DIR>/source3/runfromhere.sh ./configure
      --prefix=<INSTALL_DIR>
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
      ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
      ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
      --enable-debug
      --enable-developer
      --host=i386-pc-none
      --target=i386-pc-none
      --disable-largefile
      --disable-pie
      --enable-static=ALL
      --disable-shared
      --disable-shared-libs
      libreplace_cv_HAVE_GETADDRINFO=no
      ac_cv_file__proc_sys_kernel_core_pattern=no
      ac_cv_func_poll=no
      libreplace_cv_HAVE_MMAP=no
      ac_cv_lib_readline_rl_callback_handler_install=no
      ac_cv_func_getpassphrase=no
      ac_cv_func_sysctlbyname=no
      ac_cv_func_printf=yes
      ac_cv_func_pread=no
      ac_cv_func_pwrite=no
      ac_cv_func_pread64=no
      ac_cv_func_pwrite64=no
      ac_cv_func_posix_fallocate=no
      ac_cv_func_posix_fallocate64=no
      ac_cv_func_fdopendir=no
      ac_cv_func_mknod=no
      ac_cv_func_mknod64=no
      ac_cv_func_setgroups=no
      ac_cv_func_syslog=no
      ac_cv_func_lgetxattr=no
      ac_cv_func_lgetea=no
      ac_cv_func_attr_get=no
      ac_cv_func_llistxattr=no
      ac_cv_func_llistea=no
      ac_cv_func_removexattr=no
      ac_cv_func_removeea=no
      ac_cv_func_extattr_delete_file=no
      ac_cv_func_attr_remove=no
      ac_cv_func_lremovexattr=no
      ac_cv_func_lremoveea=no
      ac_cv_func_fremovexattr=no
      ac_cv_func_fremoveea=no
      ac_cv_func_attr_removef=no
      ac_cv_func_lsetxattr=no
      ac_cv_func_lsetea=no
      ac_cv_func_attr_set=no
      ac_cv_func_yp_get_default_domain=no
      ac_cv_func_fchmod=no
      ac_cv_func_fdatasync=no
      ac_cv_lib_rt_fdatasync=no
      ac_cv_func_posix_memalign=no
      ac_cv_func_getpeereid=no
      samba_cv_HAVE_GETPEEREID=no
      ac_cv_func_setsid=no
      fu_cv_sys_stat_statvfs=no
      ac_cv_func_dlopen=no
      ac_cv_func_shl_load=no
      ac_cv_func_shl_unload=no
      ac_cv_func_shl_findsym=no
      ac_cv_func_utime=no
      ac_cv_func_getpwent_r=no
      ac_cv_func_getgrent_r=no
      ac_cv_func_getgrouplist=no
      ac_cv_func_sigblock=no
      ac_cv_func_sigaction=no
      ac_cv_func_getgrset=no
      ac_cv_func_nanosleep=no
      ac_cv_lib_ext_rt_nanosleep=no
   BUILD_COMMAND
      <SOURCE_DIR>/source3/runfromhere.sh make
        EXEEXT=.embox.o
        libsmbclient
        bin/smbclient.embox.o
   LOG_BUILD 1
   INSTALL_COMMAND
      <SOURCE_DIR>/source3/runfromhere.sh make
        EXEEXT=.embox.o
        ""INSTALLLIBCMD_A=install""
        installlibsmbclient
        installlibtalloc
        installlibtevent
        installlibtdb
        installlibwbclient
   LOG_INSTALL 1
)
