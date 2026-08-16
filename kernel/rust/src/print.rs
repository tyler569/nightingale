use core::fmt::{Write, Result};
use crate::ffi;

pub struct KernelOutput;

impl Write for KernelOutput {
    fn write_str(&mut self, s: &str) -> Result {
        unsafe { ffi::print_view(s.as_ptr() as *const i8, s.len()) };
        Ok(())
    }
}

pub fn _print(args: core::fmt::Arguments) {
    use core::fmt::Write;
    let _ = KernelOutput.write_fmt(args);
}

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {
        $crate::print::_print(format_args!($($arg)*))
    };
}

#[macro_export]
macro_rules! println {
    () => {
        $crate::print::_print(format_args!("\n"))
    };
    ($fmt:expr $(,)?) => {
        $crate::print::_print(format_args!(concat!($fmt, "\n")))
    };
    ($fmt:expr, $($arg:tt)*) => {
        $crate::print::_print(format_args!(concat!($fmt, "\n"), $($arg)*))
    };
}
