use core::fmt::Write;

pub struct COutput;

extern "C" {
    fn raw_print(fd: i32, buf: *const u8, len: usize) -> i32;
}

impl core::fmt::Write for COutput {
    fn write_str(&mut self, string: &str) -> core::fmt::Result {
        unsafe { raw_print(0, string.as_ptr(), string.len()); }
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
