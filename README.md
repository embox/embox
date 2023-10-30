Embox [![Coverity Scan Build Status](https://scan.coverity.com/projects/700/badge.svg)](https://scan.coverity.com/projects/700)
=====

Embox is a configurable RTOS designed for resource constrained and embedded systems. Embox's main idea is to use Linux software without Linux.

Achievements
-------------
* [VoIP phone on STM32F7Discovery based on PJSIP project](https://github.com/embox/embox/wiki/PJSIP-on-STM32)
* [Qt on STM32F7Discovery](https://github.com/embox/embox/wiki/Qt-on-STM32)
* [OpenCV on STM32F769i board](https://github.com/embox/embox/wiki/OpenCV-on-STM32)
* A lot of programming languages are available: Python, Lisp, Java (phoneme), TCL, Ruby, Lua, JS, Scheme
* SSHD based on Dropbear project
* Mesa3d
* Quake3
* zeromq, qpid
* ...

Key features
---------------
* POSIX-compliant
* C++ support
* File systems: FAT, ext2/3/4, ...
* TCP/IP: BSD sockets; supported protocols: UDP, HTTP, ARP, NTP, ICMP, ...
* Cross-platform: ARM, MIPS, x86, Microblaze, SPARC, PowerPC, E2K
* Popular platforms: STM32, i.MX6, RaPi, ...
* Provides popular desktop software on MCU (e.g. STM32): Qt, OpenCV, PJSIP, dropbear, ...
* Unix-like shell utilities: ls, cat, mount, ...
* Popular programming languages: java, python, lua, tcl, lisp, ruby, js, scheme

Contacts
---------------
Mailing lists:
* main (english): embox-devel@googlegroups.com
* russian: embox-ru@googlegroups.com

Telegram chats:
* main (english): https://t.me/embox_chat_en
* news:  https://t.me/embox_news
* russian: https://t.me/embox_chat

Documentation
---------------
* [Wiki](https://github.com/embox/embox/wiki)
* [PDF](https://github.com/embox/embox-docs/releases)
* [GitHub repository with source for docs](https://github.com/embox/embox-docs)

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
For configuring it is necessary to specify parameters and modules supposed to be included into the system. There are two ways to set up a current config:

* `make confload-<template> -` build artefacts generated in a fixed place
* `./confset <template> -` build artefacts maintained in project dependent working paths

While `make confload-<template>` is used, files from the template's folder are copied into './conf' folder. You can modify them.
While `confset <template>` is used, files from the template's folder are copied into `./work/<template>/conf` folder and a link './conf' is created to `./work/<template>/conf`. This usage is suited for quick switching between your templates, because full rebuilds are avoided.
Moreover, working conf-settings are retained as well, when you switch build e.g. between real target and qemu and back again. Only downside of using the confset method: more SSD/HDD usage, dependent on number of parallel projects.

Embox has several templates prepared, to list them use the following command:
```
$ make confload
```

The simplest way to load a template is to specify its name in the command:

`$ ./confset <template>`

or
```
$ make confload-<template>
```

For the quick overview you can use one of `qemu` templates that exist for most architectures, that is, *`x86/qemu`* for x86:

`$ ./confset x86/qemu`  &nbsp; _(yet no autocomplete/-suggest when only a part is typed: to Do)_

or
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
You can use the same script with *-s -S -machine accel=tcg* flags for debugging:
```
$ sudo ./scripts/qemu/auto_qemu -s -S -machine accel=tcg
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
Embox supports networking on qemu x86, ARM, MIPS and Microblaze.
