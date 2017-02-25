
#![feature(lang_items)]
#![feature(asm)]
#![feature(const_fn)]
#![no_std]

extern crate rlibc;
extern crate spin;

mod serial;

use core::fmt::Write;
use serial::COM1;

#[allow(dead_code)]
#[derive(Debug)]
struct Test<'a> {
    s: i64,
    p: &'a i64,
}

#[no_mangle]
pub extern fn kernel_main() -> ! {
    let vga = 0xb8000 as *mut u16;
    unsafe {
        *vga.offset(10) = (b'R' as u16) | 0x4c00;
        *vga.offset(11) = (b'U' as u16) | 0x4c00;
        *vga.offset(12) = (b'S' as u16) | 0x4c00;
        *vga.offset(13) = (b'T' as u16) | 0x4c00;
    }

    write!(COM1.lock(), "Hello World: {}\n", 1234567);
    write!(COM1.lock(), "Integer:     {}\n", 1234567);
    write!(COM1.lock(), "Pointer:     {:?}\n", kernel_main as usize);
    write!(COM1.lock(), "Float:       {}\n", 1.0/3.0);
    write!(COM1.lock(), "Bad Float:   {}\n", 0.1 + 0.2):
    write!(COM1.lock(), "Structure:   {:?}\n", Test { s: 4, p: &5 });

    panic!();
}

#[cfg(not(test))]
#[lang = "eh_personality"]
extern fn eh_personality() {}

#[cfg(not(test))]
#[lang = "panic_fmt"] 
#[no_mangle]
pub extern fn panic_fmt() -> ! { loop {} }

#[allow(non_snake_case)]
#[no_mangle]
pub extern fn _Unwind_Resume() -> ! { loop {} }
