
#![feature(lang_items)]
#![no_std]

extern crate rlibc;

#[no_mangle]
pub extern fn kernel_main() -> ! {
    let vga = 0xB8_000 as *mut u16;

    let message = b"RUST!";
    for (i, c) in message.iter().enumerate() {
        unsafe {
            *vga.offset(80 + (i as isize)) = (*c as u16) | 0x4c << 8;
        }
    }

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
