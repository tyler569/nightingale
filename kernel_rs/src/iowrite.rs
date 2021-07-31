type Result = core::result::Result<(), ()>;

pub(crate) trait IoWrite {
    fn write(&mut self, buf: &[u8]) -> Result;
}

impl IoWrite for &mut [u8] {
    fn write(&mut self, buf: &[u8]) -> Result {
        for (from, to) in buf.iter().zip(self.iter_mut()) {
            *to = *from;
        }
        Ok(())
    }
}
