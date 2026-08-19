#![no_std]

extern crate alloc;

#[macro_use]
mod print;

#[macro_use]
mod init;

mod mem;
mod task;

unsafe extern "C" {
    pub fn c_panic() -> !;
}

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    unsafe { c_panic() }
}

#[unsafe(no_mangle)]
pub extern "C" fn rust_test(a: i32) {
    println!("{}", a);

    task::Task::spawn(|| println!("This is a Rust kernel thread"))
}

fn rust_init() {
    println!("Hello World from Rust init function");
}
define_init!(rust_init, 9);
