use core::fmt::{Write, Result};
use core::ptr::null;
use crate::nightingale;

pub struct COutput;

impl Write for COutput {
    fn write_str(&mut self, string: &str) -> Result {
        unsafe {
            nightingale::raw_print(null(), string.as_ptr(), string.len());
        }
        Ok(())
    }
}

pub fn global_print(args: core::fmt::Arguments) {
    COutput.write_fmt(args).expect("print failed");
}

#[macro_export]
macro_rules! print {
    () => {};
    ($fmt:expr) => {
        $crate::print::global_print(format_args!(concat!($fmt, "\0")))
    };
    ($fmt:expr, $($arg:tt)*) => {
        $crate::print::global_print(format_args!(concat!($fmt, "\0"), $($arg)*))
    };
}

#[macro_export]
macro_rules! println {
    () => {
        $crate::print!("\n")
    };
    ($fmt:expr) => {
        $crate::print!(concat!($fmt, "\n"))
    };
    ($fmt:expr, $($arg:tt)*) => {
        $crate::print!(concat!($fmt, "\n"), $($arg)*)
    };
}
