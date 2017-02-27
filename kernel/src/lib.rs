
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

use core::fmt::{ self, Write };

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
    writeln!(serial, "Structure:   {:?}", Test { s: 1234, b: '5' });
    writeln!(serial, "Multiboot pointer: {:#x}", multiboot_info);

    writeln!(serial, "Does this work?");

    let multiboot_ptr = multiboot_info as *const u32;
    let mb = multiboot2::BootInformation::new(multiboot_ptr);
    let mmap = mb.get_tag(1);
    writeln!(serial, "Here's hoping: {:?}", mmap);
    let mmap = mb.get_tag(2);
    writeln!(serial, "Here's hoping: {:?}", mmap);
    let mmap = mb.get_tag(6);
    writeln!(serial, "Here's hoping: {:?}", mmap);
    let mmap = mb.get_tag(8);
    writeln!(serial, "Here's hoping: {:?}", mmap);

    panic!();
}

#[cfg(not(test))]
#[lang = "eh_personality"]
extern fn eh_personality() {}

#[cfg(not(test))]
#[lang = "panic_fmt"] 
#[no_mangle]
pub extern fn panic_fmt(fmt: fmt::Arguments, file_line: &(&'static str, u32)) -> ! {
    loop {
        unsafe {
            asm!("cli");
            asm!("hlt");
        }
    }
}

#[allow(non_snake_case)]
#[no_mangle]
pub extern fn _Unwind_Resume() -> ! { loop {} }
