use std::path::PathBuf;

extern crate bindgen;

fn main() {
    let bindings = bindgen::Builder::default()
    .header("../../../src/compat/libc/include/string.h")
    .header("../../../src/compat/libc/include/stdio.h")
    .header("../../../src/compat/libc/include/stdlib.h")
    .clang_arg("-I../../../src/compat/posix/include")
    .clang_arg("-I../../../src/compat/libc/include")
    .clang_arg("-I../../../src/include")
    .clang_arg("-I../../../src/arch/x86/include")
    // ВАЖНО! т.к. emlibc-проект исключительно для подготовки биндингов - обязательно используем хост-цель
    .clang_arg("--target=x86_64-pc-linux-gnu")
    .ctypes_prefix("::core::ffi")
    .use_core()
    .emit_builtins()
    .raw_line("#![no_std]")
    .raw_line("#![allow(non_snake_case, non_camel_case_types, non_upper_case_globals)]")
    .generate()
    .expect("Unable to generate bindings");

    let out_path = PathBuf::from("./src");
    bindings
        .write_to_file(out_path.join("lib.rs"))
        .expect("Couldn't write bindings!");    
}