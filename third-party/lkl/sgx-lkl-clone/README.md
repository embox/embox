# Adding clone() support from sgx-lkl - how it was

Tried to apply `Add support for clone to LKL` changeset: *https://github.com/lsds/lkl/commit/3dc5bdd32415c9e4ad685fb04c7e5ade6e4c970e*. Patch did not apply cleanly.

Tried to apply it by looking to other commits from 'sgx-lkl' team and applying them first. Looked at commits between *'lsds/lkl' repo fork begining* and *clone support changeset*. Considered only those commits which affect files changed in `Add support for clone to LKL` commit.

Looks like we only need to edit and apply *https://github.com/lsds/lkl/commit/eecb2a320f3f2dd85942526df0a3d3abd402912c*
1. Needed to remove *arch/lkl/kernel/signal.c* file change. It just adds debug info into the file which refers to another feature, AFAIU. We do not have *arch/lkl/kernel/signal.c*.
2. Also, they remove `do_signal(NULL);` call from *arch/lkl/kernel/syscalls.c* file. This call was also added there in the context of another feature. We do not have this call. Let's delete this string from the `.diff` file and change `-44,9` to `-44,8`.

Already updated changeset can be applied by executing the following:
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

Clean LKL:
```
rm build/extbld/third_party/lkl/lib/.{builded,installed}
make -C build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f/tools/lkl clean
```

Additional dirty workaround: comment `do_signal(NULL);` in *build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f/arch/lkl/kernel/syscalls.c:188* (this should be removed from patches in future).

Rebuild:
```
export CFLAGS="-Wno-error" && make -j8
```

# Current status
1. Add `int cret = lkl_sys_clone(SIGCHLD, 0 , NULL, NULL, 0);` into *third-party/lkl/echotovda.c*.

2. Add these changes to *build/extbld/third_party/lkl/lib/linux-7750a5aa74f5867336949371f0e18353704d432f/tools/lkl/lib/posix-host.c*
```
void *my_function(void *data)
{
}

static lkl_thread_t thread_create_host(void* pc, void* sp, void* tls, struct lkl_tls_key* task_key, void* task_value)
{
        pthread_t thread;
        if (WARN_PTHREAD(pthread_create(&thread, NULL, my_function, NULL)))
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
lkl_sys_clone returned 39
root@embox:/#echotovda TEST
lkl_sys_clone returned 44
root@embox:/#echotovda TEST
lkl_sys_clone returned 49
root@embox:/#echotovda TEST
lkl_sys_clone returned 54
root@embox:/#echotovda TEST
lkl_sys_clone returned 59
root@embox:/#echotovda TEST
lkl_sys_clone returned 64
```
