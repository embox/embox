include(ExternalProject)
ExternalProject_Add(
   ntfs-3g
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://tuxera.com/opensource/ntfs-3g_ntfsprogs-2013.1.13.tgz
   URL_MD5 2d6fb47ddf62b51733227126fe9227fe
   UPDATE_COMMAND ""
   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/ntfs-3g/tree
      #&& bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`"
      && patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/ntfs-3g/patch.txt
      && cd <SOURCE_DIR> && autoconf && aclocal && automake
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
      ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
      ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
      --enable-debug
      --enable-mount-helper=no
      --disable-ldconfig
      --disable-largefile
      --enable-shared=no
      --with-fuse=external
      --host=i386-pc-none
   #	BUILD_COMMAND
   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
