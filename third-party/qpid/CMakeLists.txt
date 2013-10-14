include(ExternalProject)
ExternalProject_Add(
   qpid
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://apache-mirror.rbc.ru/pub/apache/qpid/0.24/qpid-cpp-0.24.tar.gz
   URL_MD5 f6245e2e68ccd1bac8fe706db3fba707
   UPDATE_COMMAND ""
   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/qpid/tree
      #&& bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`"
      && patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/qpid/patch.txt
      #&& cd <SOURCE_DIR> && autoconf && aclocal && automake
      #true
   #LOG_CONFIGURE 1
   CMAKE_CACHE_ARGS
      -DBoost_DEBUG:BOOL=on
      #-DCMAKE_SYSTEM_NAME:STRING=Embox
      -DBUILD_HA:BOOL=off
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
