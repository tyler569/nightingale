// pub fn malloc(size: usize) -> *mut c_void;
// pub fn free(ptr: *mut c_void);

use core::alloc::{GlobalAlloc, Layout};
use crate::ffi::{malloc, free};

struct CAllocator;

unsafe impl GlobalAlloc for CAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        unsafe { malloc(layout.size()) as _ }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        unsafe { free(ptr as _) }
    }
}

#[global_allocator]
static C_ALLOCATOR: CAllocator = CAllocator;
