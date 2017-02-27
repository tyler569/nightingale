
use core::marker::PhantomData;

#[derive(Debug)]
pub struct Port<T> {
    addr: u16,
    phantom: PhantomData<T>,
}

impl Port<u8> {
    pub const unsafe fn new(addr: u16) -> Port<u8> {
        Port { addr: addr, phantom: PhantomData }
    }

    #[inline]
    pub fn write(&self, value: u8) {
        unsafe {
            asm!("out $1, $0" : : "{al}"(value), "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
    }

    #[inline]
    pub fn read(&self) -> u8 {
        let value: u8;
        unsafe {
            asm!("in $0, $1" : "={al}"(value) : "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
        value
    }
}

impl Port<u16> {
    pub const unsafe fn new(addr: u16) -> Port<u16> {
        Port { addr: addr, phantom: PhantomData }
    }

    pub fn write(&self, value: u16) {
        unsafe {
            asm!("out $1, $0" : : "{ax}"(value), "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
    }

    pub fn read(&self) -> u16 {
        let value: u16;
        unsafe {
            asm!("in $0, $1" : "={ax}"(value) : "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
        value
    }
}

impl Port<u32> {
    pub const unsafe fn new(addr: u16) -> Port<u32> {
        Port { addr: addr, phantom: PhantomData }
    }

    pub fn write(&self, value: u32) {
        unsafe {
            asm!("out $1, $0" : : "{eax}"(value), "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
    }

    pub fn read(&self) -> u32 {
        let value: u32;
        unsafe {
            asm!("in $0, $1" : "={eax}"(value) : "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
        value
    }
}

