use core::{alloc::{GlobalAlloc, Layout}, ffi::c_void};
use emlibc::{memcmp, malloc, free};

pub struct MyAllocator;

unsafe impl GlobalAlloc for MyAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        let addr = malloc(layout.size() as u32);
        addr as *mut u8
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        free(ptr as *mut c_void);
    }
}

#[global_allocator]
static GLOBAL: MyAllocator = MyAllocator;

#[alloc_error_handler]
fn default_handler(layout: Layout) -> ! {
    panic!("memory allocation of {} bytes failed", layout.size())
}

#[no_mangle]
pub extern  "C" fn bcmp(
    ptr1: *const core::ffi::c_void,
    ptr2: *const core::ffi::c_void,
    n: core::ffi::c_ulong,
) -> core::ffi::c_int {
    unsafe {
        memcmp(ptr1, ptr2, n)
    }
}
