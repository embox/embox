include(ExternalProject)
ExternalProject_Add(
   STLport
   PREFIX ..
   URL http://citylan.dl.sourceforge.net/project/stlport/STLport/STLport-5.2.1/STLport-5.2.1.tar.bz2
   URL_MD5 a8341363e44d9d06a60e03215b38ddde
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   BUILD_IN_SOURCE 1
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-static --disable-shared --without-thread --without-rtti \\
         ""--with-extra-cflags=$ENV{EMBOX_DERIVED_CFLAGS}"" ""--with-extra-cxxflags=$ENV{EMBOX_DERIVED_CXXFLAGS}""
   #	BUILD_COMMAND
   LOG_BUILD 1
)
