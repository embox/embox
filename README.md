Embox [![Build Status](https://travis-ci.org/embox/embox.svg?branch=master)](https://travis-ci.org/embox/embox) [![Coverity Scan Build Status](https://scan.coverity.com/projects/700/badge.svg)](https://scan.coverity.com/projects/700)
=====
## LKL info from Anton Ostrouhhov
Clone the repository:
```
git clone https://github.com/aostrouhhov/embox.git
cd embox/
git checkout lkl-clone-support
```

Build Embox with LKL (in 32-bit x86 environment!):
```
make confload-x86/qemu
export CFLAGS="-Wno-error" && make -j8
```

Run Embox:
```
sudo ./scripts/qemu/auto_qemu
```

In Embox run `echotovda`:
```
$ echotovda TEST
```

### Add clone support from sgx-lkl
Tried to apply `Add support for clone to LKL` changeset: *https://github.com/lsds/lkl/commit/3dc5bdd32415c9e4ad685fb04c7e5ade6e4c970e*. Patch did not apply cleanly.

Tried to apply it by looking to other commits from 'sgx-lkl' team and applying them first. Looked at commits between *'lsds/lkl' repo fork begining* and *clone support changeset*. Considered only those commits which affect files changed in `Add support for clone to LKL` commit.

Looks like we only need to edit and apply *https://github.com/lsds/lkl/commit/eecb2a320f3f2dd85942526df0a3d3abd402912c*
1. Needed to remove *arch/lkl/kernel/signal.c* file change. It just adds debug info into the file which refers to another feature, AFAIU. We do not have *arch/lkl/kernel/signal.c*.
2. Also, they remove `do_signal(NULL);` call from *arch/lkl/kernel/syscalls.c* file. This call was also added there in the context of another feature. We do not have this call. Let's delete this string from the `.diff` file and change `-44,9` to `-44,8`.

Already updated changeset can be applied by exucting the following:
```
patch -d $HOME/embox/build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f -p1 < $HOME/embox/third-party/lkl/sgx-lkl-clone/patch-1.diff
```

Now let's apply unmodified *3dc5bd*:
```
patch -d $HOME/embox/build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f -p1 < $HOME/embox/third-party/lkl/sgx-lkl-clone/patch-2.diff
```
It should fail to patch 2 files. Edit them manually:
1. *arch/lkl/Kconfig* - add `select HAVE_COPY_THREAD_TLS` to the end of `config LKL` section.
2. *arch/lkl/include/uapi/asm/unistd.h* - add `#define __ARCH_WANT_SYS_CLONE`.

Remove some build artifacts **each time you modify LKL**:
```
rm build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f/tools/lkl/lib/lkl.o build/extbld/third_party/lkl/lib/.installed build/extbld/third_party/lkl/lib/.builded
```

Also, particulary this patch requires `make distclean` (first time I see such behaviour):
```
(cd build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f && make distclean)
```

Additional dirty workaround: comment `do_signal(NULL);` in *build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f/arch/lkl/kernel/syscalls.c:188* (this should be removed from patches in future).

Rebuild:
```
export CFLAGS="-Wno-error" && make -j8
```

### Current status
First, add `int cret = lkl_sys_clone(SIGCHLD, 0 , NULL, NULL, 0);` into *third-party/lkl/echotovda.c*. Then:

1. In case we add these changes to *build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f/tools/lkl/lib/posix-host.c*
```
static lkl_thread_t thread_create_host(void* pc, void* sp, void* tls, struct lkl_tls_key* task_key, void* task_value)
{
        // The same as 'thread_create'
        pthread_t thread;
        if (WARN_PTHREAD(pthread_create(&thread, NULL, NULL, NULL)))
                return 0;
        else
                return (lkl_thread_t) thread;
}

static void thread_destroy_host(lkl_thread_t tid, struct lkl_tls_key* task_key)
{
    // Just temporary stub
}

 struct lkl_host_operations lkl_host_ops = {
    ...
	.thread_create_host = thread_create_host,
	.thread_destroy_host = thread_destroy_host,
    ...
```
we get the following output:
```
root@embox:/#echotovda TEST                                                     
pthread_create(&thread, NULL, NULL, NULL): Try again later


  ______
 |  ____|                                            __          __
 | |___  _ __ ___            ____  ____  ____  _____/ /   _____ / /
 |  ___|| '_ ` _ \          / __ \/ __ \/ __ \/ ___/ /   |_____| |
 | |____| | | | | |_ _ _   / /_/ / /_/ / /_/ (__  )_/    |_____| |
 |______|_| |_| |_(_|_|_)  \____/\____/ .___/____(_)           | |
                                     /_/                        \_\
 ASSERTION FAILED on CPU 0
	at ./src/include/kernel/spinlock.h:92
	in function __spin_trylock

lock->owner != cpu_id

	(Recursive lock of a spin owned by this CPU)


 --   00001000 * A R    thread 70  task 3 -------------------------------------

 19 0x0051e45e <__assertion_handle_failure+0xf6> 
 18 0x004f89cc <__spin_trylock+0x48> 
 17 0x004f8b48 <spin_trylock+0x16> 
 16 0x004f8b7e <spin_lock_ipl+0x20> 
 15 0x004f8c03 <waitq_add+0x3e> 

...
```
2. In case we don't touch *build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f/tools/lkl/lib/posix-host.c* (don't add `thread_create_host` and `thread_destroy_host`), we get the following output:
```
root@embox:/#echotovda TEST                                                     
EXCEPTION [0x6]: error = 00000000
EAX=20000000    EBX=008000c0 ECX=3529804d EDX=007bab60
 GS=00000010     FS=00000010  ES=00000010  DS=00000010
EDI=010d7e8c    ESI=058ae540 EBP=00000000 EIP=0080a229
 CS=00000008 EFLAGS=00000a47 ESP=010d7e84  SS=058ae540


 --   00001000 * A R    thread 70  task 3 -------------------------------------

  3 0x004f3a10 <exception_handler+0x12a> 
  2 0x00100079 <excep_stub+0x1c> 
  1 0x0080a229 <__security_initcall_start+0x2e01> 
  ...
```

### To see your changes (diff) in LKL

1. Download and extract clean LKL, apply all Embox patches:
```
mkdir $HOME/clean-lkl
wget --directory-prefix=$HOME/clean-lkl https://github.com/lkl/linux/archive/7750a5aa74f5867336949371f0e18353704d432f.zip

unzip $HOME/clean-lkl/7750a5aa74f5867336949371f0e18353704d432f.zip -d $HOME/clean-lkl
rm $HOME/clean-lkl/7750a5aa74f5867336949371f0e18353704d432f.zip

patch -d $HOME/clean-lkl/ -p0 < $HOME/embox/third-party/lkl/lkl-i386-support.patch
patch -d $HOME/clean-lkl/ -p0 < $HOME/embox/third-party/lkl/lkl-embox-support.patch
```

2. Now it is possible to see actual diff (assuming you are in the root dir of embox repo):
```
diff -X third-party/lkl/lkl-diff-excludes.txt -ur $HOME/clean-lkl/linux-7750a5aa74f5867336949371f0e18353704d432f build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f
```
---

## LKL specific info

LKL can be built only in x86 environment (not x86-64).
Suitable docker image may be used, i.e. `embox/emdocker-lkl` have all necessary packages installed.
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
```
rm build/extbld/third_party/lkl/lib/.{builded,installed}
make -C build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f/tools/lkl clean
```

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
