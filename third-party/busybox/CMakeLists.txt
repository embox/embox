include(ExternalProject)
ExternalProject_Add(
   busybox
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://www.busybox.net/downloads/busybox-1.21.1.tar.bz2
   URL_MD5 795394f83903b5eec6567d51eebb417e
   UPDATE_COMMAND ""
   PATCH_COMMAND
      true
      #cd $ENV{EMBOX_ROOT}/third-party/samba/tree &&
      #bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`" &&
      #patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/samba/patch.txt
   BUILD_IN_SOURCE 1
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
     make defconfig
   BUILD_COMMAND
     make
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
      ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
      ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
   LOG_BUILD 1
   INSTALL_COMMAND
   	true
   LOG_INSTALL 1
)
