#![no_std]
#![no_main]
#![feature(
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

extern crate core;
extern crate alloc;

use core::{panic::PanicInfo, ffi::CStr, slice::from_raw_parts};
use alloc::ffi::CString;
use emlibc::std::*;

static STR1: &[u8] = b"Predefined sliced string";
static ERROR: &[u8] = b"Error!";

#[no_mangle]
pub unsafe fn main(_argc: i32, _argv: *const *const i8) {

    let args = from_raw_parts(_argv, _argc as usize);

    let c_str = CString::new(STR1).unwrap();
    println!("Test1: {}", "Inlined string");
    println!("Test2: {}", c_str.to_str().unwrap());

    for (idx, &arg) in args.iter().enumerate() {
        println!("arg[{}]: {}", idx, CStr::from_ptr(arg as *const i8).to_str().unwrap());
    }

    ()
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
