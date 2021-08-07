#![no_std]
#![feature(alloc_error_handler)]
#![feature(atomic_mut_ptr)]
#![feature(asm)]

#![allow(unused)]
#![deny(warnings)]

extern crate alloc;

use crate::spawn::JoinHandle;

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
        let handle = spawn::spawn(|| {
            println!("Let's do some math in this thread");
            sleep_thread(1000);
            2 + 2
        });


        let mutex = alloc::sync::Arc::new(sync::Mutex::new(0i32));
        unsafe { mutex.init(); }
        let mut handles = alloc::vec::Vec::new();
        for _ in 0..10 {
            let mutex = mutex.clone();
            handles.push(spawn::spawn(move || {
                for _ in 0..100 {
                    let mut guard = mutex.lock();
                    *guard += 1;
                }
            }));
        }
        for handle in handles.into_iter() {
            handle.join();
        }
        println!("And we ended up with: {}", *mutex.lock());


        if let Ok(value) = handle.join() {
            println!("Got the math back: {}", *value);
        } else {
            println!("Got an error back :(");
        }
    });
    println!("This is the println!() macro");
}

