include(ExternalProject)
ExternalProject_Add(
   gcc
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://mirrors.ustc.edu.cn/gnu/gcc/gcc-4.8.1/gcc-4.8.1.tar.bz2
   URL_MD5 3b2386c114cd74185aa3754b58a79304
   #URL http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/gcc/gcc-4.6.2/gcc-4.6.2.tar.bz2
   #URL_MD5 028115c4fbfb6cfd75d6369f4a90d87e
   UPDATE_COMMAND ""
   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/gcc/tree
      && bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`"
      #&& patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/gcc/patch.txt
      #&& cd <SOURCE_DIR> && autoconf && aclocal && automake
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      sh <SOURCE_DIR>/configure
         --prefix=<INSTALL_DIR>
         ""CC_FOR_TARGET=/usr/bin/gcc""
         ""CXX_FOR_TARGET=/usr/bin/g++""
         ""CFLAGS_FOR_TARGET=$ENV{EMBOX_DERIVED_CFLAGS}""
         ""CXXFLAGS_FOR_TARGET=$ENV{EMBOX_DERIVED_CXXFLAGS}""
         ""LDFLAGS_FOR_TARGET=$ENV{EMBOX_DERIVED_LDFLAGS}""
         --enable-languages=c,c++
         #--target=i386-pc-msdosdjgpp
         --target=i386-pc-elf
         --disable-shared
         --disable-tls
         --disable-multilib
         --disable-nls
         #--disable-gcc
   BUILD_COMMAND
      make
         GCC_FOR_TARGET=gcc
         CC_FOR_TARGET=gcc
         ""TARGET-libiberty=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         #""TARGET-build-libiberty=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         #""TARGET-build-fixincludes=LINKER=true\ CC=true\ AR=true\ all""
         ""TARGET-zlib=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         ""TARGET-libbacktrace=LINKER=true\ CC=true\ AR=touch\ LINK=true\ all""
         ""TARGET-libcpp=LINKER=true\ CC=true\ CXX=true\ POSTCOMPILE=true\ AR=true\ RANLIB=touch\ all""
         ""TARGET-libdecnumber=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         ""TARGET-fixincludes=LINKER=true\ CC=true\ AR=true\ all""
         ""TARGET-lto-plugin=CC=true\ LIBTOOL=true\ all""
         ""TARGET-gcc=LINKER=true\ CC=true\ CXX=true\ GCC_FOR_TARGET=gcc\ AR=true\ RANLIB=true\ libgcc-support""
         all-target-libgcc
   INSTALL_COMMAND
      false
   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
