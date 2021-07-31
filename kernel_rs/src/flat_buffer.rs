use core::fmt::Write;
use crate::iowrite::IoWrite;

pub struct FlatBuffer<'a>(pub &'a mut [u8]);

impl<'a> Write for FlatBuffer<'a> {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        self.0.write(s.as_bytes());
        Ok(())
    }
}
