
use core::fmt;
use spin::Mutex;

// TEMPORARY: make an interface to ports
fn out_port(addr: u16, value: u8) {
    unsafe {
        asm!("out $1, $0" : : "{al}"(value), "{dx}"(addr) : "memory" : "intel", "volatile");
    }
}

fn in_port(addr: u16) -> u8 {
    let value: u8;
    unsafe {
        asm!("in $0, $1" : "={al}"(value) : "{dx}"(addr) : "memory" : "intel", "volatile");
    }
    return value; 
}

pub static COM1: Mutex<SerialPort> = Mutex::new(SerialPort { addr: 0x3f8 });

#[derive(Debug)]
pub struct SerialPort {
    addr: u16,
}

impl SerialPort {
    pub fn new(addr: u16) -> SerialPort {
        SerialPort { addr: addr }
    }

    pub fn init(&self) {
        out_port(self.addr + 1, 0x00);
        out_port(self.addr + 3, 0x80);
        out_port(self.addr + 0, 0x03);
        out_port(self.addr + 1, 0x00);
        out_port(self.addr + 3, 0x03);
        out_port(self.addr + 2, 0xc7);
        out_port(self.addr + 4, 0x0b);
    }

    #[allow(dead_code)]
    fn received(&self) -> bool {
        if in_port(self.addr + 5) & 0x01 > 0 {
            true
        } else {
            false
        }
    }

    fn buf_empty(&self) -> bool {
        if in_port(self.addr + 5) & 0x20 > 0 {
            true
        } else {
            false
        }
    }
 
    pub fn send(&self, data: u8) {
        while ! self.buf_empty() {}
        out_port(self.addr, data);
    }

    #[allow(dead_code)]
    pub fn recv(&self) -> u8 {
        while ! self.received() {}
        in_port(self.addr)
    }
}

impl fmt::Write for SerialPort {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for c in s.bytes() {
            if c == b'\n' {
                self.send(b'\r');
            }
            self.send(c);
        }
        Ok(())
    }
}

