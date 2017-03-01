
// Nightingale
// A kernel for x86_64
// Copyright (C) 2017, Tyler Philbrick
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


use core::fmt;
use spin::Mutex;
use llio::Port;

pub static COM1: Mutex<SerialPort> = Mutex::new(SerialPort::new(0x3f8));

#[derive(Debug)]
pub struct SerialPort {
    data: Port<u8>,
    int_enable: Port<u8>,
    fifo_ctrl: Port<u8>,
    line_ctrl: Port<u8>,
    modem_ctrl: Port<u8>,
    line_status: Port<u8>,
    modem_status: Port<u8>,
    scratch: Port<u8>,
}

// Line status:
const DATA_READY: u8 = 0x01;
const OVERRUN_ERROR: u8 = 0x02;
const PARITY_ERROR: u8 = 0x04;
const FRAMING_ERROR: u8 = 0x08;
const BREAK_INT: u8 = 0x10;
const CAN_TRANSMIT: u8 = 0x20;
const TRANSMIT_EMPTY: u8 = 0x40;
const FIFO_ERROR: u8 = 0x80;


impl SerialPort {
    const fn new(addr: u16) -> SerialPort {
        unsafe {
            SerialPort {
                data: Port::<u8>::new(addr),
                int_enable: Port::<u8>::new(addr + 1),
                fifo_ctrl: Port::<u8>::new(addr + 2),
                line_ctrl: Port::<u8>::new(addr + 3),
                modem_ctrl: Port::<u8>::new(addr + 4),
                line_status: Port::<u8>::new(addr + 5),
                modem_status: Port::<u8>::new(addr + 6),
                scratch: Port::<u8>::new(addr + 7),
            }
        }
    }

    pub fn init(&self) {
        // TODO: useful constants
        self.int_enable.write(0x00);
        self.line_ctrl.write(0x80);
        self.data.write(0x03);
        self.int_enable.write(0x00);
        self.line_ctrl.write(0x03);
        self.fifo_ctrl.write(0xc7);
        self.modem_ctrl.write(0x0b);
    }

    fn line_status_includes(&self, status: u8) -> bool {
        (self.line_status.read() & status) != 0
    }
 
    fn send(&self, data: u8) {
        while ! self.line_status_includes(CAN_TRANSMIT) {}
        self.data.write(data);
    }

    #[allow(dead_code)]
    fn recv(&self) -> u8 {
        while ! self.line_status_includes(DATA_READY) {}
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

