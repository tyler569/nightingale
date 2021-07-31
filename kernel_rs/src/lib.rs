#![no_std]
#![feature(alloc_error_handler)]

extern crate alloc;

use alloc::alloc::{GlobalAlloc, Layout};
use core::panic::PanicInfo;

mod spawn;

struct KernelAllocator;

#[global_allocator]
static GLOBAL_ALLOCATOR: KernelAllocator = KernelAllocator{};

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
    fn printf(format: *const u8, ...) -> i32;
    fn sleep_thread(ms: i32);
}

#[no_mangle]
pub unsafe extern fn rust_main() {
    printf("Hello World from Rust: %#x\n\0".as_ptr(), 0x1337);
    spawn::spawn(|| {
        printf("And this is a rust kernel thread!\n\0".as_ptr());
        let handle = spawn::spawn(|| {
            printf("Let's do some math in this thread\n\0".as_ptr());
            sleep_thread(1000);
            2 + 2
        });
        if let Ok(value) = handle.join() {
            printf("Got the math back: %i\n\0".as_ptr(), *value);
        } else {
            printf("Got an error back :(\n\0".as_ptr());
        }
    });
}

extern "C" {
    fn break_point();
    fn disable_irqs();
    fn halt() -> !;
}

#[panic_handler]
fn panic(_panic_info: &PanicInfo) -> ! {
    unsafe {
        break_point();
        disable_irqs();
        printf(b"[PANIC] panic from Rust\n\0".as_ptr());
        halt();
    }
}

