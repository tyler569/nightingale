use core::fmt::{Write, Result};
use crate::ffi;

pub struct KernelOutput;

impl Write for KernelOutput {
    fn write_str(&mut self, s: &str) -> Result {
        unsafe { ffi::print_view(s.as_ptr() as *const i8, s.len()) };
        Ok(())
    }
}

