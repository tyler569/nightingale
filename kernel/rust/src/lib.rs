#![no_std]

extern crate alloc;

#[macro_use]
mod print;

#[macro_use]
mod init;

mod mem;
mod task;
mod vm;

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

    task::Task::spawn(|| println!("This is a Rust kernel thread"));

    const ADDR: usize = 0xffff_fffe_0000_0000;
    unsafe { vm::create_unbacked(ADDR, 0x1000, vm::PAGE_WRITABLE) };
    let ptr = ADDR as *mut i32;
    unsafe { *ptr = 1 };
    println!("The ptr {:?} has a value of {}", ptr, unsafe { *ptr });
}

fn rust_init() {
    println!("Hello World from Rust init function");
}
define_init!(rust_init, 9);
