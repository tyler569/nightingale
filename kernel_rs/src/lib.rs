#![no_std]
#![feature(alloc_error_handler)]
#![feature(atomic_mut_ptr)]

extern crate alloc;

use alloc::alloc::{GlobalAlloc, Layout};
use core::panic::PanicInfo;
use crate::spawn::JoinHandle;

#[macro_use]
mod print;
mod iowrite;
mod ffi;
mod filesystem;
mod flat_buffer;
mod spawn;
mod sync;
mod syscall_rs;

struct KernelAllocator;

#[global_allocator]
static GLOBAL_ALLOCATOR: KernelAllocator = KernelAllocator;

extern "C" {
    fn malloc(size: usize) -> *mut u8;
    fn free(ptr: *mut u8);
}

unsafe impl GlobalAlloc for KernelAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        malloc(layout.size())
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        free(ptr);
    }
}

#[alloc_error_handler]
fn alloc_error(l: Layout) -> ! {
    panic!("Allocation error allocating {:?}", l);
}

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

extern "C" {
    fn break_point();
    fn disable_irqs();
    fn halt() -> !;
}

#[panic_handler]
fn panic(panic_info: &PanicInfo) -> ! {
    unsafe {
        break_point();
        disable_irqs();
        println!("[PANIC] panic from Rust: {:?}", panic_info);
        halt();
    }
}
