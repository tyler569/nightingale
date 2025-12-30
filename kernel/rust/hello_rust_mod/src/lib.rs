#![no_std]
#![no_main]

use nightingale_kernel::*;

fn init_module(_mod: *const Mod) -> i32 {
    unsafe {
        ffi::printf(cstr!("Hello World from Rust kernel module!\n"));
    }
    MODINIT_SUCCESS
}

kernel_module! {
    name: "hello_rust_mod",
    init: init_module,
}
