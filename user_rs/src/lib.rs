#![no_std]
#![feature(asm)]

#![allow(unused)]
#![deny(warnings)]

pub mod syscall;

extern "C" {
    fn main(argc: isize, argv: *const *const i8) -> isize;
}

#[no_mangle]
pub unsafe extern "C" fn _start(argc: isize, argv: *const *const i8) -> isize {
    main(argc, argv);
    syscall::c_syscall1(syscall::_EXIT, 0);
    0
}

use core::panic::PanicInfo;

#[panic_handler]
fn my_panic(info: &PanicInfo) -> ! {
    loop {}
}
