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
   #BUILD_IN_SOURCE 1
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
   BUILD_COMMAND
      <SOURCE_DIR>/source3/runfromhere.sh make libsmbclient
   LOG_BUILD 1
   INSTALL_COMMAND
      <SOURCE_DIR>/source3/runfromhere.sh make
        ""INSTALLLIBCMD_A=install""
        installlibsmbclient
   LOG_INSTALL 1
)
