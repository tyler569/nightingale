
use core::fmt;
use spin::Mutex;
use llio::portio::PortIO;

pub static COM1: Mutex<SerialPort> = Mutex::new(SerialPort::new(0x3f8));

#[derive(Debug)]
pub struct SerialPort {
    data: PortIO<u8>,
    int_enable: PortIO<u8>,
    int_id: PortIO<u8>,
    line_ctrl: PortIO<u8>,
    modem_ctrl: PortIO<u8>,
    line_status: PortIO<u8>,
    modem_status: PortIO<u8>,
    scratch: PortIO<u8>,
}

impl SerialPort {
    const fn new(addr: u16) -> SerialPort {
        SerialPort {
            data: PortIO::new(addr),
            int_enable: PortIO::new(addr + 1),
            int_id: PortIO::new(addr + 2),
            line_ctrl: PortIO::new(addr + 3),
            modem_ctrl: PortIO::new(addr + 4),
            line_status: PortIO::new(addr + 5),
            modem_status: PortIO::new(addr + 6),
            scratch: PortIO::new(addr + 7),
        }
    }

    fn init(&self) {
        // TODO: useful constants
        self.int_enable.write(0x00);
        self.line_ctrl.write(0x80);
        self.data.write(0x03);
        self.int_enable.write(0x00);
        self.line_ctrl.write(0x03);
        self.int_id.write(0xc7);
        self.modem_ctrl.write(0x0b);
    }

    #[allow(dead_code)]
    fn received(&self) -> bool {
        if self.line_status.read() & 1 > 0 {
            true
        } else {
            false
        }
    }

    fn buf_empty(&self) -> bool {
        if self.line_status.read() & 0x20 > 0 {
            true
        } else {
            false
        }
    }
 
    pub fn send(&self, data: u8) {
        while ! self.buf_empty() {}
        self.data.write(data);
    }

    #[allow(dead_code)]
    pub fn recv(&self) -> u8 {
        while ! self.received() {}
        self.data.read()
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

