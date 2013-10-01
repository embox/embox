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
         ""CFLAGS_FOR_TARGET=$ENV{EMBOX_DERIVED_CFLAGS}\ -DLIBGCC2_HAS_XF_MODE=0\ -I<BINARY_DIR>/gcc/include""
         ""CXXFLAGS_FOR_TARGET=$ENV{EMBOX_DERIVED_CXXFLAGS}\ -I<BINARY_DIR>/gcc/include""
         ""LDFLAGS_FOR_TARGET=$ENV{EMBOX_DERIVED_LDFLAGS}""
         --enable-languages=c,c++
         --target=$ENV{EMBOX_DERIVED_TARGET}
         --disable-shared
         --disable-tls
         --disable-multilib
         --disable-nls
         #--disable-gcc
   BUILD_COMMAND
      make
         GCC_FOR_TARGET=gcc
         CXX_FOR_TARGET=g++
         RAW_CXX_FOR_TARGET=g++
         CC_FOR_TARGET=gcc
         AR_FOR_TARGET=ar
         RANLIB_FOR_TARGET=ranlib
         ""TARGET-libiberty=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         #""TARGET-build-libiberty=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         #""TARGET-build-fixincludes=LINKER=true\ CC=true\ AR=true\ all""
         ""TARGET-zlib=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         ""TARGET-libbacktrace=LINKER=true\ CC=true\ AR=touch\ 'LINK=mkdir\ -p\ .libs&&true'\ 'LIBS=&&touch\ .libs/libbacktrace.a'\ all""
         ""TARGET-libcpp=LINKER=true\ CC=true\ CXX=true\ POSTCOMPILE=true\ AR=true\ RANLIB=touch\ all""
         ""TARGET-libdecnumber=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         ""TARGET-fixincludes=LINKER=true\ CC=true\ AR=true\ all""
         ""TARGET-lto-plugin=CC=true\ LIBTOOL=true\ all""
         ""TARGET-gcc=LINKER=true\ CC=true\ CXX=true\ GCC_FOR_TARGET=gcc\ AR=true\ RANLIB=true\ libgcc-support\ stmp-int-hdrs""
         ""TARGET-target-libstdc++-v3='SUBDIRS=include\ libsupc++'\ all-recursive""
         all-target-libgcc
         all-target-libstdc++-v3
   INSTALL_COMMAND
      make
         GCC_FOR_TARGET=gcc
         CXX_FOR_TARGET=g++
         RAW_CXX_FOR_TARGET=g++
         CC_FOR_TARGET=gcc
         AR_FOR_TARGET=ar
         RANLIB_FOR_TARGET=ranlib
         ""TARGET-libiberty=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         #""TARGET-build-libiberty=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         #""TARGET-build-fixincludes=LINKER=true\ CC=true\ AR=true\ all""
         ""TARGET-zlib=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         ""TARGET-libbacktrace=LINKER=true\ CC=true\ AR=touch\ 'LINK=mkdir\ -p\ .libs&&true'\ 'LIBS=&&touch\ .libs/libbacktrace.a'\ all""
         ""TARGET-libcpp=LINKER=true\ CC=true\ CXX=true\ POSTCOMPILE=true\ AR=true\ RANLIB=touch\ all""
         ""TARGET-libdecnumber=LINKER=true\ CC=true\ AR=true\ RANLIB=touch\ all""
         ""TARGET-fixincludes=LINKER=true\ CC=true\ AR=true\ all""
         ""TARGET-lto-plugin=CC=true\ LIBTOOL=true\ all""
         ""TARGET-gcc=LINKER=true\ CC=true\ CXX=true\ GCC_FOR_TARGET=gcc\ AR=true\ RANLIB=true\ libgcc-support\ stmp-int-hdrs""
         ""TARGET-target-libstdc++-v3=SUBDIRS=libsupc++\ install-recursive""
         install-target-libgcc
         all-target-libstdc++-v3
   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
