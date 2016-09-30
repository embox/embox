
cmake_minimum_required(VERSION 2.8)

include(ExternalProject)
ExternalProject_Add(
   unixbyte
   PREFIX .
   DOWNLOAD_DIR $ENV{EMBOX_ROOT}/download
   URL https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/byte-unixbench/UnixBench5.1.3.tgz

   URL_MD5 21edc4a9e41ad1f9b0297d7b6d45c99a

   UPDATE_COMMAND ""

   PATCH_COMMAND
      patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/unixbyte/patch.txt

   CONFIGURE_COMMAND
      rm -r <SOURCE_DIR>/../unixbyte-build
      && cp -r <SOURCE_DIR> <SOURCE_DIR>/../unixbyte-build

   BUILD_COMMAND
      make
      ""CC=$ENV{EMBOX_DERIVED_CC}""
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""

   INSTALL_COMMAND ""

   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
