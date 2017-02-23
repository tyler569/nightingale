
#![feature(lang_items)]
#![no_std]

extern crate rlibc;

mod debug;

#[no_mangle]
pub extern fn kernel_main() -> ! {
    debug::raw_print(160, 0x4c, b"RUST IS THE BEST");
    debug::raw_print_num(240, 0x4c, kernel_main as u64);

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
