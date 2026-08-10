#![no_std]

mod ffi;

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    unsafe { ffi::panic(c"panic from rust".as_ptr()) }
}

#[unsafe(no_mangle)]
pub extern "C" fn rust_test(a: i32) {
    unsafe { ffi::printf(c"%i\n".as_ptr(), a); }
}
