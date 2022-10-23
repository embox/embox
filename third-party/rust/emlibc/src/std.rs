/*
   Для реализации аналогов макросов println и т.п. за основу взят код из:
   https://github.com/mmastrac/rust-libc-print
 */
#![no_std]

pub use super::libc_dbg as dbg;
pub use super::libc_eprint as eprint;
pub use super::libc_eprintln as eprintln;
pub use super::libc_print as print;
pub use super::libc_println as println;

use core::{alloc::{GlobalAlloc, Layout},
             ffi::c_void,
             convert::TryFrom,
             file,
             line,
             stringify};

use super::{memcmp, malloc, free, write};

#[doc(hidden)]
pub const __LIBC_NEWLINE: &str = "\n";
#[doc(hidden)]
pub const __LIBC_STDOUT: i32 = 1;
#[doc(hidden)]
pub const __LIBC_STDERR: i32 = 2;

pub struct Allocator;

unsafe impl GlobalAlloc for Allocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        let addr = malloc(layout.size().try_into().unwrap());
        addr as *mut u8
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        free(ptr as *mut c_void);
    }
}

#[global_allocator]
static GLOBAL: Allocator = Allocator;

#[cfg(not(test))]
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

#[doc(hidden)]
pub struct __LibCWriter(i32);

impl core::fmt::Write for __LibCWriter {
    #[inline]
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        __libc_println(self.0, s)
    }
}

impl __LibCWriter {
    #[inline]
    pub fn new(handle: i32) -> __LibCWriter {
        __LibCWriter(handle)
    }

    #[inline]
    pub fn write_fmt(&mut self, args: core::fmt::Arguments) -> core::fmt::Result {
        core::fmt::Write::write_fmt(self, args)
    }

    #[inline]
    pub fn write_str(&mut self, s: &str) -> core::fmt::Result {
        __libc_println(self.0, s)
    }

    #[inline]
    pub fn write_nl(&mut self) -> core::fmt::Result {
        __libc_println(self.0, __LIBC_NEWLINE)
    }
}

#[doc(hidden)]
#[inline]
pub fn __libc_println(handle: i32, msg: &str) -> core::fmt::Result {
    let msg = msg.as_bytes();

    let mut written = 0;
    while written < msg.len() {
        match unsafe { libc_write(handle, &msg[written..]) } {
            // Ignore errors
            None | Some(0) => break,
            Some(res) => written += res,
        }
    }

    Ok(())
}

unsafe fn libc_write(handle: i32, bytes: &[u8]) -> Option<usize> {
    usize::try_from(unsafe {
        write(
            handle,
            bytes.as_ptr().cast::<core::ffi::c_void>(),
            bytes.len().try_into().unwrap(),
        )
    })
    .ok()
}


/// Macro for printing to the standard output, with a newline.
///
/// Does not panic on failure to write - instead silently ignores errors.
///
/// See [`println!`](https://doc.rust-lang.org/std/macro.println.html) for
/// full documentation.
///
/// You may wish to `use libc_print::std_name::*` to use a replacement
/// `println!` macro instead of this longer name.
#[macro_export]
macro_rules! libc_println {
    () => { $crate::libc_println!("") };
    ($($arg:tt)*) => {
        #[allow(unused_must_use)]
        {
            let mut stm = $crate::std::__LibCWriter::new($crate::std::__LIBC_STDOUT);
            stm.write_fmt(format_args!($($arg)*));
            stm.write_nl();
        }
    };
}

/// Macro for printing to the standard output.
///
/// Does not panic on failure to write - instead silently ignores errors.
///
/// See [`print!`](https://doc.rust-lang.org/std/macro.print.html) for
/// full documentation.
///
/// You may wish to `use libc_print::std_name::*` to use a replacement
/// `print!` macro instead of this longer name.
#[macro_export]
macro_rules! libc_print {
    ($($arg:tt)*) => {
        #[allow(unused_must_use)]
        {
            let mut stm = $crate::std::__LibCWriter::new($crate::std::__LIBC_STDOUT);
            stm.write_fmt(format_args!($($arg)*));
        }
    };
}

/// Macro for printing to the standard error, with a newline.
///
/// Does not panic on failure to write - instead silently ignores errors.
///
/// See [`eprintln!`](https://doc.rust-lang.org/std/macro.eprintln.html) for
/// full documentation.
///
/// You may wish to `use libc_print::std_name::*` to use a replacement
/// `eprintln!` macro instead of this longer name.
#[macro_export]
macro_rules! libc_eprintln {
    () => { $crate::libc_eprintln!("") };
    ($($arg:tt)*) => {
        #[allow(unused_must_use)]
        {
            let mut stm = $crate::std::__LibCWriter::new($crate::std::__LIBC_STDERR);
            stm.write_fmt(format_args!($($arg)*));
            stm.write_nl();
        }
    };
}

/// Macro for printing to the standard error.
///
/// Does not panic on failure to write - instead silently ignores errors.
///
/// See [`eprint!`](https://doc.rust-lang.org/std/macro.eprint.html) for
/// full documentation.
///
/// You may wish to `use libc_print::std_name::*` to use a replacement
/// `eprint!` macro instead of this longer name.
#[macro_export]
macro_rules! libc_eprint {
    ($($arg:tt)*) => {
        #[allow(unused_must_use)]
        {
            let mut stm = $crate::std::__LibCWriter::new($crate::std::__LIBC_STDERR);
            stm.write_fmt(format_args!($($arg)*));
        }
    };
}

/// Macro for printing a static string to the standard output.
///
/// Does not panic on failure to write - instead silently ignores errors.
#[macro_export]
macro_rules! libc_write {
    ($arg:expr) => {
        #[allow(unused_must_use)]
        {
            let mut stm = $crate::std::__LibCWriter::new($crate::std::__LIBC_STDOUT);
            stm.write_str($arg);
        }
    };
}

/// Macro for printing a static string to the standard error.
///
/// Does not panic on failure to write - instead silently ignores errors.
#[macro_export]
macro_rules! libc_ewrite {
    ($arg:expr) => {
        #[allow(unused_must_use)]
        {
            let mut stm = $crate::std::__LibCWriter::new($crate::std::__LIBC_STDERR);
            stm.write_str($arg);
        }
    };
}

/// Macro for printing a static string to the standard output, with a newline.
///
/// Does not panic on failure to write - instead silently ignores errors.
#[macro_export]
macro_rules! libc_writeln {
    ($arg:expr) => {
        #[allow(unused_must_use)]
        {
            let mut stm = $crate::std::__LibCWriter::new($crate::std::__LIBC_STDOUT);
            stm.write_str($arg);
            stm.write_nl();
        }
    };
}

/// Macro for printing a static string to the standard error, with a newline.
///
/// Does not panic on failure to write - instead silently ignores errors.
#[macro_export]
macro_rules! libc_ewriteln {
    ($arg:expr) => {
        #[allow(unused_must_use)]
        {
            let mut stm = $crate::std::__LibCWriter::new($crate::std::__LIBC_STDERR);
            stm.write_str($arg);
            stm.write_nl();
        }
    };
}

/// Prints and returns the value of a given expression for quick and dirty
/// debugging.
///
/// An example:
///
/// ```rust
/// let a = 2;
/// let b = dbg!(a * 2) + 1;
/// //      ^-- prints: [src/main.rs:2] a * 2 = 4
/// assert_eq!(b, 5);
/// ```
///
/// See [dbg!](https://doc.rust-lang.org/std/macro.dbg.html) for full documentation.
///
/// You may wish to `use libc_print::std_name::*` to use a replacement
/// `dbg!` macro instead of this longer name.
#[macro_export]
macro_rules! libc_dbg {
    () => {
        $crate::libc_eprintln!("[{}:{}]", $file!(), $line!())
    };
    ($val:expr $(,)?) => {
        match $val {
            tmp => {
                $crate::libc_eprintln!("[{}:{}] {} = {:#?}", file!(), line!(), stringify!($val), &tmp);
                tmp
            }
        }
    };
    ($($val:expr),+ $(,)?) => {
        ($($crate::libc_dbg!($val)),+,)
    };
}

#[cfg(test)]
mod tests {
    use super::{eprintln, println};
    
    #[test]
    fn test_stdout() {
        crate::libc_println!("stdout fd = {}", crate::std::__LIBC_STDOUT);
    }

    #[test]
    fn test_stderr() {
        crate::libc_eprintln!("stderr fd = {}", crate::std::__LIBC_STDERR);
    }

    #[test]
    fn test_stdout_write() {
        crate::libc_writeln!("stdout!");
    }

    #[test]
    fn test_stderr_write() {
        crate::libc_ewriteln!("stderr!");
    }

    #[test]
    fn test_dbg() {
        let a = 2;
        let b = libc_dbg!(a * 2) + 1;
        assert_eq!(b, 5);
    }

    #[test]
    fn test_println() {
        println!("stdout fd = {}", crate::std::__LIBC_STDOUT);
    }

    #[test]
    fn test_eprintln() {
        eprintln!("stderr fd = {}", crate::std::__LIBC_STDERR);
    }
}