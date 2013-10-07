include(ExternalProject)
ExternalProject_Add(
   boost
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://optimate.dl.sourceforge.net/project/boost/boost/1.54.0/boost_1_54_0.tar.gz
   URL_MD5 efbfbff5a85a9330951f243d0a46e4b9
   UPDATE_COMMAND ""
   PATCH_COMMAND
      #cd $ENV{EMBOX_ROOT}/third-party/boost/tree &&
      #bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`" &&
      patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/boost/patch.txt &&
      true
   BUILD_IN_SOURCE 1
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
     ./bootstrap.sh
       --prefix=<INSTALL_DIR>
       --without-libraries=chrono,coroutine,filesystem,locale,regex,graph,iostreams,thread,math
   BUILD_COMMAND
     ./b2
       -q
       #-d 12
       ""cflags=$ENV{EMBOX_DERIVED_CFLAGS}""
       ""cxxflags=$ENV{EMBOX_DERIVED_CXXFLAGS}""
       ""linkflags=$ENV{EMBOX_DERIVED_LDFLAGS}""
       variant=debug
       link=static
       threading=multi
       address-model=32
       #define=BOOST_DATE_TIME_NO_LOCALE
       #define=BOOST_NO_STD_LOCALE
       #define=BOOST_LEXICAL_CAST_ASSUME_C_LOCALE
       #define=BOOST_NO_STD_WSTRING
       define=BOOST_HAS_THREADS
       define=BOOST_HAS_PTHREADS
       define=BOOST_HAS_SCHED_YIELD
       define=BOOST_HAS_GETTIMEOFDAY
       define=BOOST_LOG_WITHOUT_SYSLOG
       #define=BOOST_NO_CWCTYPE
       define=_POSIX_TIMERS=1
   LOG_BUILD 1
   INSTALL_COMMAND
   	false
   LOG_INSTALL 1
)
