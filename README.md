Embox [![Build Status](https://travis-ci.org/embox/embox.svg?branch=master)](https://travis-ci.org/embox/embox) [![Coverity Scan Build Status](https://scan.coverity.com/projects/700/badge.svg)](https://scan.coverity.com/projects/700)
=====

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
$ sudo apt-get install build-essential gcc-multilib curl libmpc-dev
```

For Arch Linux:
```
$ sudo pacman -S make gcc-multilib cpio curl
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

#### Notes on Mac OS X build
MacOS's default gcc produces Mach-O binaries, which are unusable outside from MacOS. Crosscompiler should be used in order to build elf. Clone https://github.com/embox/crosstool and make
```
$ ./crosstool.sh i386
```
Then modify `CROSS_COMPILE` variable in `conf/build.conf` like
```
CROSS_COMPILE = /path/to/toolchain/bin/i386-elf-
```

If you've installed toolchain in some directory from `$PATH`, then `CROSS_COMPILE` could be just
```
CROSS_COMPILE = i386-elf-
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
