#![no_std]

mod ffi;
mod print;

use core::fmt::Write;

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    unsafe { ffi::c_panic() }
}

#[unsafe(no_mangle)]
pub extern "C" fn rust_test(a: i32) {
    _ = writeln!(print::KernelOutput, "{}", a);
}
