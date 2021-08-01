#![no_std]
#![feature(start)]

use user_rs::syscall;

#[start]
fn main(argc: isize, argv: *const *const u8) -> isize {
    let string = "Hello World\n";
    unsafe {
        syscall::c_syscall3(syscall::WRITE, 1, string.as_ptr() as usize, string.len());
    }
    0
}
