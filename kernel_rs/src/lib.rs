#![no_std]
#![feature(alloc_error_handler)]
#![feature(atomic_mut_ptr)]
#![feature(asm)]

#![allow(unused)]
#![deny(warnings)]

extern crate alloc;

use alloc::vec::Vec;
use crate::sync::Mutex;

#[macro_use]
mod print;

mod ng_alloc;
// mod context;
mod io_write;
// mod ipc;
mod ffi;
mod filesystem;
mod flat_buffer;
mod panic;
mod spawn;
mod sync;
mod syscall_rs;

extern "C" {
    fn sleep_thread(ms: i32);
}

#[no_mangle]
pub unsafe extern fn rust_main() {
    println!("Hello World from Rust: {:x}", 0x1337);
    spawn::spawn(|| {
        println!("And this is a rust kernel thread!");
        spawn::spawn(|| {
            println!("Let's do some math in this thread");
            sleep_thread(1000);
            println!("Math is {}", 2 + 2);
        });


        let mutex = Mutex::new_arc(0i32);
        for _ in 0..10 {
            let mutex = mutex.clone();
            spawn::spawn(move || {
                for _ in 0..100 {
                    let mut guard = mutex.lock();
                    *guard += 1;
                }
            });
        }
        sleep_thread(1000);
        println!("And we ended up with: {}", *mutex.lock());
    });
}

