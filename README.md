Embox [![Build Status](https://travis-ci.org/embox/embox.svg?branch=master)](https://travis-ci.org/embox/embox) [![Coverity Scan Build Status](https://scan.coverity.com/projects/700/badge.svg)](https://scan.coverity.com/projects/700)
=====
## LKL specific info

This subsystem provides binary compatibility layer with GNU/Linux executables.

The idea is to make [Linux Kernel Library](https://github.com/lkl/linux) act like a paravirtualized Linux kernel inside of Embox. This makes it possible to redirect detected Linux syscalls into LKL and process them. For more info see ["LKL Subsystem"](https://github.com/embox/embox/wiki/LKL-subsystem) wiki page.

### Development environment
Currently, the work takes place in `lkl` branch.

LKL can be built only in x86 environment (not x86-64). To deploy x86-32 Virtual Machine with Vagrant:
```
vagrant up
vagrant ssh
```

Also, suitable docker image may be used, i.e. `embox/emdocker-lkl` have all necessary packages installed.
It's based on `i386/ubuntu`, see https://github.com/embox/emdocker/blob/09df170db6aab92eb7172c2d417c76436cebdc82/lkl/Dockerfile)

Build environment:
```
docker run -it --rm -u $(id -u):$(id -g) -v $PWD:/ws -w /ws --detach-keys ctrl-_ embox/emdocker-lkl
```

Test (QEMU) environment:
```
docker run -it --privileged --rm -v $PWD:/ws -w /ws --detach-keys ctrl-_ embox/emdocker-lkl
```

### Clean Linux in LKL
If changes were made to LKL code, do this before rebuilding Embox image:
```
rm build/extbld/third_party/lkl/lib/.{builded,installed}
make -C build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f/tools/lkl clean
```

### Check changes made to LKL
In case you want to see changes made to LKL, do the folowing.

1. Download and extract clean LKL, apply all Embox patches:
```
mkdir $HOME/clean-lkl

wget --directory-prefix=$HOME/clean-lkl https://github.com/lkl/linux/archive/7750a5aa74f5867336949371f0e18353704d432f.zip

unzip $HOME/clean-lkl/7750a5aa74f5867336949371f0e18353704d432f.zip -d $HOME/clean-lkl

rm $HOME/clean-lkl/7750a5aa74f5867336949371f0e18353704d432f.zip

patch -d $HOME/clean-lkl/ -p0 < /embox/third-party/lkl/lkl-i386-support.patch

patch -d $HOME/clean-lkl/ -p0 < /embox/third-party/lkl/lkl-embox-support.patch

patch -d $HOME/clean-lkl/ -p0 < /embox/third-party/lkl/lkl-tls-enhancement.patch
```

2. Now it is possible to see actual diff (assuming you are in the root dir of embox repo):
```
diff -X third-party/lkl/lkl-diff-excludes.txt -ur $HOME/clean-lkl/linux-7750a5aa74f5867336949371f0e18353704d432f build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f
```

### Getting started
Clone the repository and switch to `lkl` branch:
```
git clone https://github.com/embox/embox.git
cd embox
git checkout lkl
```

Deploy Virtual Machine:
```
vagrant up
vagrant ssh
```

Build Embox (it takes some time due to LKL):
```
cd /embox
make confload-x86/lkl-qemu
export CFLAGS="-Wno-error" && make -j8
```

Compile pure Linux binaries:
```
gcc -nostdlib -emain -fpie -N -o conf/rootfs/linux_echo third-party/lkl/linux_echo.c
gcc -nostdlib -emain -fpie -N -o conf/rootfs/linux_ls third-party/lkl/linux_ls.c
gcc -nostdlib -emain -fpie -N -o conf/rootfs/linux_cat third-party/lkl/linux_cat.c
```

Make sure they work in a GNU/Linux environment:
```
./conf/rootfs/linux_echo "Hello, World!"
./conf/rootfs/linux_ls /proc
./conf/rootfs/linux_cat /proc/devices
```

Rebuild Embox to update rootfs, run QEMU:
```
export CFLAGS="-Wno-error" && make -j8
sudo ./scripts/qemu/auto_qemu
```

Make sure Linux binaries work in Embox with LKL:
```
load_app linux_echo "Hello, World!"
load_app linux_ls /proc
load_app linux_cat /proc/devices
load_app linux_cat /proc/slabinfo
...
```

`/proc/slabinfo` is suitable to check how LKL's `/vda` device (which translates output from LKL to Embox terminal) works with huge output.

Please note that not everything should work. For example, `/proc/cpuinfo` won't show anything because its content should be implemented manually.

### Links

* https://github.com/lsds/sgx-lkl
* https://github.com/lsds/sgx-lkl/blob/d8716cf49e32bee002853d6d979003300b92f66a/src/main/load_elf.c
* https://github.com/bediger4000/userlandexec
* https://www.microsoft.com/en-us/research/uploads/prod/2018/08/Confidential_Computing_Peter_Pietzuch_Manuel_Costa.pdf
* https://storage.googleapis.com/pub-tools-public-publication-data/pdf/6ece8c450ee375eb31b2795a2a227a5d8c3598b7.pdf

---

Embox is a configurable operating system kernel designed for resource
constrained and embedded systems.

Getting started
---------------
Here's a quick overview on how to build and run Embox.

Required environment:
 - `gcc` and `make`
 - cross compiler for the target platform

### Preparing environment
For Debian-based systems (most packages are installed out of box though):
```
$ sudo apt-get install build-essential gcc-multilib curl libmpc-dev python
```

For Arch Linux:
```
$ sudo pacman -S make gcc-multilib cpio qemu
```

For MAC OS X (requires [MacPorts](https://www.macports.org/install.php) installed):
```
$ sudo port install i386-elf-gcc i386-elf-binutils cpio gawk qemu
```

For any system with Docker (more info on wiki [Emdocker](https://github.com/embox/embox/wiki/Emdocker)):
```
$ ./scripts/docker/docker_start.sh
$ . ./scripts/docker/docker_rc.sh
```

### Building Embox
First of all:
```
$ git clone https://github.com/embox/embox.git embox
$ cd embox
```
Since Embox is highly configurable project, it is necessary to specify modules to be built and params for them. To build the OS `make` command is used.
All commands described below are called from the `embox` directory, which includes `src/`, `templates/`, ...

#### Configuring the project
For configuring it is needed to specify params and modules supposed to be included into the system. Embox has several templates prepared, to list them use the following command:
```
$ make confload
```

The simplest way to load a template is to specify its name in the command:
```
$ make confload-<template>
```

For the quick overview you can use one of `qemu` templates that exist for most architectures, that is, *`x86/qemu`* for x86:
```
$ make confload-x86/qemu
```

#### Building the image
After configuring the project just run `make` to build:
```
$ make
```

### Running on QEMU
The resulting image can now be run on QEMU. The simplest way is to execute `./scripts/qemu/auto_qemu` script:
```
$ sudo ./scripts/qemu/auto_qemu
```
`sudo` is requried to setup a TUN/TAP device necessary to emulate networking.

After the system is loaded, you’ll see the `embox>` prompt, now you are able to run commands.
For example, `help` lists all existing commands.

To test the connection:
```
ping 10.0.2.16
```
If everything's fine, you can connect to the Embox terminal via `telnet`.

To exit Qemu type <kbd>ctrl + A</kbd> and <kbd>X</kbd> after that.

### Debugging
You can use the same script with *-s -S -no-kvm* flags for debugging:
```
$ sudo ./scripts/qemu/auto_qemu -s -S -no-kvm
```
After running that QEMU waits for a connection from a gdb-client. Run gdb in the other terminal:
```
$ gdb ./build/base/bin/embox
...
(gdb) target extended-remote :1234
(gdb) continue
```
The system starts to load.

At any moment in gdb terminal you can type <kbd>ctrl + C</kbd> and see the stack of the current thread (`backtrace`) or set breakpoints (`break <function name>`, `break <file name>:<line number>`).

### Other architectures
Embox supports the following CPU architectures: x86, ARM, Microblaze, SPARC, PPC, MIPS.

In order to work with architectures other than x86 you'll need a cross compiler.
After installing the cross compiler just repeat the step above, starting with configuring:
```
make confload-<arch>/qemu
make
sudo ./scripts/qemu/auto_qemu
```
The output is quite the same as in the case of the x86 architecture.

#### Networking support
Embox supports networking on x86, ARM, and Microblaze.
