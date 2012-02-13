#!/bin/bash

target_name=arm-elf
target_configure_options="--enable-interwork --with-float=soft"

binutils_name=binutils-2.22
gcc_name=gcc-4.6.2
gmp_name=gmp-5.0.2
mpc_name=mpc-0.9
mpfr_name=mpfr-3.1.0


if [ ! -e "$binutils_name".tar.bz2 ]
then
	wget http://ftp.gnu.org/gnu/binutils/$binutils_name.tar.bz2
fi

#tar xjf $binutils_name.tar.bz2

#mkdir build-binutils
#cd build-binutils
#../$binutils_name/configure --prefix=/tmp/$target_name-$gcc_name \
#--target=$target_name --disable-werror --disable-nls
#make && make install
#cd ..

if [ ! -e "$gmp_name".tar.bz2 ]
then
	wget ftp://ftp.gmplib.org/pub/$gmp_name/$gmp_name.tar.bz2
fi

if [ ! -e "$gmp_name".tar.bz2 ]
then
	wget ftp://ftp.gmplib.org/pub/$gmp_name/$gmp_name.tar.bz2
fi
if [ ! -e "$mpc_name".tar.gz ]
then
	wget http://www.multiprecision.org/mpc/download/$mpc_name.tar.gz
fi
if [ ! -e "$mpfr_name".tar.bz2 ]
then
	wget http://www.mpfr.org/mpfr-current/$mpfr_name.tar.bz2
fi

if [ ! -e $gcc_name.tar.bz2 ]
then
	wget http://gcc.cybermirror.org/releases/$gcc_name/$gcc_name.tar.bz2
fi


#tar xjf $gmp_name.tar.bz2
#tar xjf $mpfr_name.tar.bz2
#tar xjf $gcc_name.tar.bz2
#tar xzf $mpc_name.tar.gz


#ln -s ../$gmp_name $gcc_name/gmp
#ln -s ../$mpc_name $gcc_name/mpc
#ln -s ../$mpfr_name $gcc_name/mpfr

#mkdir build-gcc
cd build-gcc
#../$gcc_name/configure --prefix=/tmp/$target_name-$gcc_name --target=$target_name --disable-werror \
# --with-gnu-ld  --disable-nls --disable-multilib --with-gnu-as   --without-headers  \
# --enable-languages=c --disable-libssp --with-mpfr-include=$(pwd)/../$gcc_name/mpfr/src \
# --with-mpfr-lib=$(pwd)/mpfr/src/.libs  $target_configure_options
make && make install
cd ..


#pushd /tmp > /dev/null
#tar cvf - arm-elf-gcc-4.4.4 | bzip2 -f > arm-elf-gcc-4.4.4.tar.bz2
#rm -fr arm-elf
#popd > /dev/null
