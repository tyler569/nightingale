#![no_std]
#![feature(alloc_error_handler)]
#![feature(atomic_mut_ptr)]
#![allow(unused)]

extern crate alloc;

#[macro_use]
mod print;
mod nightingale;
mod ng_alloc;
mod panic;

#[no_mangle]
pub unsafe extern fn rust_main() {
    println!("Hello World from Rust: {:x}", 0x1234);
}

