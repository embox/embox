include(ExternalProject)
ExternalProject_Add(
   libgcc
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://mirrors.ustc.edu.cn/gnu/gcc/gcc-4.8.1/gcc-4.8.1.tar.bz2
   URL_MD5 3b2386c114cd74185aa3754b58a79304
   #URL http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/gcc/gcc-4.6.2/gcc-4.6.2.tar.bz2
   #URL_MD5 028115c4fbfb6cfd75d6369f4a90d87e
   UPDATE_COMMAND ""
   PATCH_COMMAND
      true
      #cd $ENV{EMBOX_ROOT}/third-party/ntfs-3g/tree
      #&& bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`"
      #&& patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/ntfs-3g/patch.txt
      #&& cd <SOURCE_DIR> && autoconf && aclocal && automake
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      sh <SOURCE_DIR>/libgcc/configure
         --prefix=<INSTALL_DIR>
         ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
         ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
         ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
         --host=i386-pc-elf
         --disable-shared
         --disable-tls
         --disable-multilib
   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
