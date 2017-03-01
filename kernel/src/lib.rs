
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


#![feature(lang_items)]
#![feature(asm)]
#![feature(const_fn)]
#![no_std]

extern crate rlibc;
extern crate spin;

mod memory;
mod multiboot2;
mod llio;
mod serial;

use core::fmt::{Write};

#[allow(dead_code)]
#[derive(Debug)]
struct Test {
    s: i64,
    b: char,
}

#[no_mangle]
pub extern fn kernel_main(multiboot_info: u32) -> ! {
    let vga = 0xb8000 as *mut u16;
    unsafe {
        *vga.offset(10) = (b'R' as u16) | 0x4c00;
        *vga.offset(11) = (b'U' as u16) | 0x4c00;
        *vga.offset(12) = (b'S' as u16) | 0x4c00;
        *vga.offset(13) = (b'T' as u16) | 0x4c00;
    }

    // memory::init();
    
    let mut serial = serial::COM1.lock();
    serial.init();
    writeln!(serial, "Structure:   {:?}", Test { s: 1234, b: '5' }).unwrap();
    writeln!(serial, "Multiboot pointer: {:#x}", multiboot_info).unwrap();

    writeln!(serial, "Does this work?").unwrap();

    let multiboot_ptr = multiboot_info as *const u32;
    let mb = unsafe { multiboot2::BootInformation::new(multiboot_ptr) };
    let mmap = mb.get_tag(1);
    writeln!(serial, "Here's hoping: {:?}", mmap).unwrap();
    let mmap = mb.get_tag(2);
    writeln!(serial, "Here's hoping: {:?}", mmap).unwrap();
    let mmap = mb.get_tag(6);
    writeln!(serial, "Here's hoping: {:?}", mmap).unwrap();
    let mmap = mb.get_tag(8);
    writeln!(serial, "Here's hoping: {:?}", mmap).unwrap();

    panic!();
}

#[cfg(not(test))]
#[lang = "eh_personality"]
extern fn eh_personality() {}

#[cfg(not(test))]
#[allow(unused_variables)]
#[lang = "panic_fmt"] 
#[no_mangle]
pub extern fn panic_fmt() -> ! {
    loop {
        unsafe {
            asm!("cli");
            asm!("hlt");
        }
    }
}

#[allow(non_snake_case)]
#[no_mangle]
pub extern fn _Unwind_Resume() -> ! { panic!() }

