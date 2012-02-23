#!/bin/bash

#arm
#this flags was recommended by debian --with-arch=armv4 --with-no-thumb-interwork
#target_name=arm-elf
#target_configure_options="--enable-interwork --with-float=soft"


#microblaze platform was added in gcc 4.6
#target_name=microblaze-elf
#target_configure_options=


#sparc
#leon platform was added in 4.6 (sparc-leon-) and so on it means the same if use --with-tune=leon
#also --with-cpu=leon was added  to libgloss (binutils)
#target_name=sparc-elf
#target_configure_options="--with-tune=leon --with-cpu=leon"
#target_configure_options="--with-tune=leon --with-cpu=v8"
target_name=sparc-leon-elf
target_configure_options=--with-soft-fp

prefix_dir=/tmp
binutils_name=binutils-2.22
gcc_name=gcc-4.6.2
gmp_name=gmp-5.0.2
mpc_name=mpc-0.9
mpfr_name=mpfr-3.1.0


if [ ! -e "$binutils_name".tar.bz2 ]
then
	wget http://ftp.gnu.org/gnu/binutils/$binutils_name.tar.bz2
fi

tar xjf $binutils_name.tar.bz2

mkdir build-binutils
cd build-binutils
../$binutils_name/configure --prefix=$prefix_dir/$target_name-$gcc_name --target=$target_name \
 --disable-werror --disable-nls
make && make install
cd ..

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

tar xjf $gcc_name.tar.bz2
tar xjf $gmp_name.tar.bz2
tar xjf $mpfr_name.tar.bz2
tar xzf $mpc_name.tar.gz


ln -s ../$gmp_name $gcc_name/gmp
ln -s ../$mpc_name $gcc_name/mpc
ln -s ../$mpfr_name $gcc_name/mpfr

mkdir build-gcc
cd build-gcc
../$gcc_name/configure --prefix=$prefix_dir/$target_name-$gcc_name --target=$target_name --disable-werror \
 --with-gnu-ld  --disable-nls --disable-multilib --with-gnu-as   --without-headers  \
 --enable-languages=c --disable-libssp --with-mpfr-include=$(pwd)/../$gcc_name/mpfr/src \
 --with-mpfr-lib=$(pwd)/mpfr/src/.libs  $target_configure_options
make && make install
cd ..

#gdb_name=gdb-7.4

#if [ ! -e $gdb_name.tar.bz2 ]
#then
#	wget http://ftp.gnu.org/gnu/gdb/$gdb_name.tar.bz2
#fi

#tar xjf $gdb_name.tar.bz2

#ln -s ../$gmp_name $gdb_name/gmp
#ln -s ../$mpc_name $gdb_name/mpc
#ln -s ../$mpfr_name $gdb_name/mpfr

#mkdir build-gdb
#cd build-gdb
#../$gdb_name/configure -prefix=$prefix_dir/$target_name-$gcc_name --target=$target_name
#make && make install
#cd ..

#pushd /tmp > /dev/null
#tar cvf - arm-elf-gcc-4.4.4 | bzip2 -f > arm-elf-gcc-4.4.4.tar.bz2
#rm -fr arm-elf
#popd > /dev/null
