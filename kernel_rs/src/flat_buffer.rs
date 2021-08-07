use core::fmt::Write;
use crate::io_write::IoWrite;

pub struct FlatBuffer<'a>(pub &'a mut [u8]);

impl<'a> Write for FlatBuffer<'a> {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        match self.0.write(s.as_bytes()) {
            Ok(_) => Ok(()),
            Err(_) => Err(core::fmt::Error),
        }
    }
}
