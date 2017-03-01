
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


use core::marker::PhantomData;

#[derive(Debug)]
pub struct Port<T> {
    addr: u16,
    phantom: PhantomData<T>,
}

#[allow(dead_code)]
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
        let value;
        unsafe {
            asm!("in $0, $1" : "={al}"(value) : "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
        value
    }
}

#[allow(dead_code)]
impl Port<u16> {
    pub const unsafe fn new(addr: u16) -> Port<u16> {
        Port { addr: addr, phantom: PhantomData }
    }

    #[inline]
    pub fn write(&self, value: u16) {
        unsafe {
            asm!("out $1, $0" : : "{ax}"(value), "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
    }

    #[inline]
    pub fn read(&self) -> u16 {
        let value: u16;
        unsafe {
            asm!("in $0, $1" : "={ax}"(value) : "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
        value
    }
}

#[allow(dead_code)]
impl Port<u32> {
    pub const unsafe fn new(addr: u16) -> Port<u32> {
        Port { addr: addr, phantom: PhantomData }
    }

    #[inline]
    pub fn write(&self, value: u32) {
        unsafe {
            asm!("out $1, $0" : : "{eax}"(value), "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
    }

    #[inline]
    pub fn read(&self) -> u32 {
        let value: u32;
        unsafe {
            asm!("in $0, $1" : "={eax}"(value) : "{dx}"(self.addr) :
                 "memory" : "intel", "volatile");
        }
        value
    }
}

