#![no_std]
#![no_main]
#![feature(
    alloc_error_handler,
    naked_functions,
    lang_items,
    panic_info_message
)]
#![allow(
    dead_code,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals
)]

mod utils;

extern crate core;
extern crate alloc;

use core::{panic::PanicInfo};
use alloc::ffi::CString;

static STR1: &[u8] = b"Console Test 1";
static ERROR: &[u8] = b"Error!";

#[no_mangle]
pub unsafe fn main(stack_top: *const u8) {
    // TODO: попробовать использовать аргументы
    let argc = *(stack_top as *const u32);
    let argv = stack_top.add(8) as *const *const u8;

    use core::slice::from_raw_parts as mkslice;
    let _args = mkslice(argv, argc as usize);

    let c_str = CString::new(STR1).unwrap();
    emlibc::printf(c_str.as_ptr() as *const i8);
    emlibc::printf(b"234, %s (%d)\0".as_ptr() as *const i8, b"567890\0".as_ptr() as *const i8, 42i32);
    ()
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
