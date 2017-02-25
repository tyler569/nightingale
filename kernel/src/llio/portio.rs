
struct PortIO<T> {
    addr: u16,
}

impl PortIO<u8> {
    fn new(addr: u16) -> PortIO<u8> {
        PortIO { addr: addr }
    }

    fn write(&self, value: u8) {
        unsafe {
            asm!("out $1, $0" : : "{al}"(value), "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
    }

    fn read(&self) -> u8 {
        let value: u8;
        unsafe {
            asm!("in $0, $1" : "={al}"(value) : "{dx}"(addr) :
                 "memory" : "intel", "volatile");
        }
        value
    }
}

